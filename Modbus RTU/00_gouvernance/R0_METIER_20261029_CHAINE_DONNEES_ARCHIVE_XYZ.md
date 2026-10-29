# MSM TR2 — R0-MÉTIER — Chaîne des données XYZ, décimation et fichier microSD

**Statut : CONCEPTION CANDIDATE / ANALYSE R0, NON GELÉE, NON IMPLÉMENTÉE, NON QUALIFIÉE.** 29 octobre 2026. Responsable : ChatGPT. Référence : `loloLR17/MSM_vibe/main`, lecture directe des sources à HEAD `c79f8e02fb9c04b9b070472dc11bf04beffdb352`. Cette note développe le [cahier des charges métier R0](R0_METIER_20261029_CAHIER_DES_CHARGES.md) et l'[étude SPI2](R0_METIER_20261029_ETUDE_MICROSD_SPI2.md) sans modifier les spécifications V1/V1.1/LIVE R0 gelées.

## 1. Contrat métier et principe de séparation

Exigences déjà arbitrées : capture XYZ continue sans lacune silencieuse, microSD amovible lisible après retrait sur PC, 8 h obligatoires/24 h souhaitables, ±8 g nominal, **2 000 échantillons/s/axe enregistrés** comme cible, 2–25 Hz prioritaire et 2–700 Hz en bande élargie, **composante continue conservée**, six capteurs possibles sans mesure de déphasage intercapteurs en V1. V1.5 IP/PoE reste à étudier ; pas d'implémentation réseau en V1.

```text
IIS3DWB XYZ 16 bits, ODR natif ~26,667 kHz, FIFO 512 mots de 7 B
       |
       +--> décodage TAG, reconstruction séquence/temps, saturation, qualité
                     |
                     +--> branche PRIORITAIRE ARCHIVE (sans retrait DC)
                     |     filtre numérique passe-bas anti-repliement
                     |     + rééchantillonnage -> ~2 000 XYZ/s
                     |     + index/temps/qualité -> file RAM bornée
                     |     -> blocs CRC -> écriture carte SPI2
                     |     -> fichier sur système de fichiers PC -> import PC
                     |
                     +--> branche SECONDAIRE LIVE (indépendante)
                           AC1 passe-haut / RMS / crêtes / B3 ou extension
                           sans transformer le contenu de l'archive
```

La « donnée brute archivée » au sens métier signifie XYZ non amputés du DC/gravité, **mais après filtrage et rééchantillonnage** : ce n'est **pas** une copie à 26,7 kHz de chaque mot natif. Les composantes au-delà de la bande préservée sont **irréversiblement** retirées ; documenter ce fait et la réponse du filtre. Ne jamais étiqueter ce flux comme « raw 26,7 kHz ». Les entiers 16 bits signés compacts décrivent le contenu proposé du fichier, **sans changement des règles d'encodage Modbus V1**.

## 2. Acquisition physique : réutilisation et limites exactes

- Capteur ST IIS3DWB : sortie native XYZ 16 bits, ODR annoncé **26,667 kHz**, plage ±2/4/8/16 g, filtre LPF1 interne ~6,3 kHz. Le FIFO comporte **512 mots de 7 octets**, soit **3 584 octets totaux dont 3 072 octets de données de mesures** lorsque les mots sont XYZ ; capacité temporelle idéale `512 / 26667 ≈ 19,2 ms`, **avant watermark, occupation, latence de drainage, perturbations et insertion éventuelle de timestamps/température**. Ce chiffre ne fixe pas une marge opérationnelle admissible.
- Le décodage des tags et la chronologie des mots (accéléromètre, timestamp, éventuellement température) sont obligatoires. Le code `experiments/iis3dwb_st_reference` et `platform/stm32/fifo_event_time.c` ont déjà une logique de reconstruction temporelle par anchors/counter ; réutiliser en contrôlant ses conditions d'intégration et ses comportements à l'overflow, **pas** générer des timestamps au moment des appels SPI.
- **Attention constructeur à vérifier sur banc :** AN5444 recommande **SPI mode 0** pour les lectures FIFO ; en mode 3, les phases `CS=0 && SCK=1` doivent rester **≤25 µs**. Le code `platform/stm32/main.c` choisit mode 3 dans plusieurs profils de bench FIFO. Ne pas modifier silencieusement les profils qualifiés ; auditer les transactions réelles et comparer avec mode 0 uniquement si l'hypothèse apporte un test utile.
- **Chaîne capteur pour archivage DC :** garder `FDS=0` (voie passe-bas) et désactiver les corrections d'offset matérielles non maîtrisées. Si `LPF2_XL_EN=0`, la bande dépend de LPF1 ~6,3 kHz ; un filtre anti-repliement logiciel restera donc indispensable. LPF2 interne `ODR/20 ≈ 1,33 kHz` pourrait aider, mais ne garantit pas seul l'atténuation nécessaire à 1 kHz, tandis que `ODR/45 ≈ 593 Hz` risque d'atténuer la bande 700 Hz. **Ne pas geler ces registres sans essais gain/phase/amplitude**.
- Chaque session doit capturer les registres effectifs, WHO_AM_I, FS actif relu, sensibilité/composantes d'offset appliquées ou non, erreurs de tag, overrun, saturation et identité physique.

**Sources constructeur :** [IIS3DWB DS12569](https://www.st.com/resource/en/datasheet/iis3dwb.pdf) ; [AN5444 Rev3, §§3 et 6](https://www.st.com/resource/en/application_note/an5444-iis3dwb-ultrawide-bandwidth-lownoise-3axis-digital-vibration-sensor-stmicroelectronics.pdf).

## 3. Filtrage anti-repliement + changement de cadence

### 3.1 Pourquoi le découpage naïf ne convient pas

- Conserver un échantillon sur treize à partir de 26 667 Hz donne ~**2 051,3 Hz**, pas 2 000 Hz ; surtout, **aucun sous-échantillonnage direct ne rejette le repliement spectral** des fréquences hautes.
- À `f_out=2000 Hz`, Nyquist vaut 1 000 Hz. Pour préserver 700 Hz, la bande de transition utile pour l'anti-repliement va **en première approximation de 700 à 1000 Hz** ; l'atténuation au-dessus de 1 kHz doit être suffisante pour empêcher les composantes mécaniques jusqu'aux ~6,3 kHz du capteur de polluer 0–1 kHz après décimation. Les tolérances de gain et de rejet doivent être **spécifiées et mesurées**, pas seulement déclarées « FIR ».
- La voie archive doit préserver le DC (gain à fréquence zéro proche de l'unité et absence de passe-haut). Un FIR passe-bas à phase linéaire, éventuellement **multistade / polyphasé**, est un **candidat d'architecture**, pas un algorithme gelé. La latence de groupe et les transitoires d'amorçage/arrêt doivent être traçables.

### 3.2 Rapport de fréquences et horloge

`26 666,67 / 2 000 ≈ 40/3`. Un ré-échantillonnage rationnel nominal **3/40** est une piste si la cadence réellement observée du capteur correspond assez étroitement à cette valeur. Le constructeur indique 26,667 kHz, **pas une garantie d'égalité exacte à 80 000/3 Hz ni une fréquence absolue sans dérive**. Un rapport fixe non vérifié peut donner des échantillons de sortie régulièrement espacés dans l'horloge capteur mais une durée réelle légèrement différente de celle calculée avec l'étiquette 2 000 Hz.

**Contrat temporel candidat :** mesurer les anchors du capteur et l'horloge de référence locale, dériver un temps monotone par index d'échantillon source, définir une grille de sortie à cadence cible et garder une relation vérifiable entre indices source/sortie et temps. Choisir entre (a) rapport nominal fixe + fréquence réellement estimée stockée, si l'écart répond à la tolérance métier, ou (b) ré-échantillonnage à ratio ajusté de manière bornée si l'objectif est 2 000 Hz par seconde réelle ; **qualification obligatoire de l'absence de dérive, de saut et de jitter**. La précision intercapteurs de phase est hors V1.

**Discontinuité :** sur tag manquant, overrun, reset, saturation invalidante ou horloge incohérente, publier une entrée d'anomalie et la quantité connue/estimée de perte (ou « inconnue »), interrompre ou réinitialiser l'état de filtrage selon le cas, démarrer un segment correctement indexé. **Interdit de créer des échantillons interpolés et de les présenter comme mesurés**, sauf politique future explicitement versionnée et signalée.

### 3.3 Banc de preuve DSP avant portage STM32

Implémentation de référence *hôte* indépendante et comparaison bit à bit ou à tolérance définie avec le calcul embarqué à quantification identique ; signaux tests DC/gravité/échelon, 2/5/25/100/500/700/800/1000/1500/3000/6000 Hz, multisinusoïdes, bruit, amplitudes proches ±8g, filtres à saturation, début/fin de campagne, pertes/timestamps anormaux et dérive d'ODR. Relever en particulier réponse en amplitude/phase/délai, gain DC, rejet d'alias, sensibilité numérique et CPU/RAM/CYCLES worst-case. **Aucune fréquence de coupure ni longueur FIR ne sont présentées comme qualifiées** à ce jour.

### 3.4 Pré-dimensionnement chiffré : candidat FIR à deux étages /10 puis ×3/4

**Nature des résultats : calculs de conception et simulation numérique hôte non versée au dépôt, PAS firmware testé, PAS métrologie physique qualifiée.** Cet exemple sert à établir qu'un filtrage 2–700 Hz est plausiblement réalisable sans refonte du MCU, et à donner une référence reproductible à R1-DSP.

| Étage | Entrée nominale | Transformation | Filtre FIR candidat | Cible d'atténuation |
|---|---:|---|---|---|
| A — anti-repliement avant /10 | 80 000/3 = 26 666,666… Hz | décimer par 10 -> 8 000/3 = 2 666,666… Hz | **107 taps**, FIR linéaire fenêtre de Kaiser, bande conservée 0–700 Hz, arrêt cible à **1 966,667 Hz** | protéger 0–700 Hz des replis de l'étage /10 ; le premier repli qui rejoint 700 Hz est à `f_sortie_A - 700` |
| B — ré-échantillonnage polyphasé | 8 000/3 Hz | interpoler par 3 (grille 8 000 Hz), filtrer puis décimer par 4 -> **2 000 Hz** | **69 taps** sur grille interpolée 8 kHz, passe jusqu'à 700 Hz, arrêt cible à **1 300 Hz** | protéger 0–700 Hz des replis de l'étage /4 ; le premier repli qui rejoint 700 Hz est à `f_sortie_B - 700` |

**Pourquoi ces bords d'arrêt ?** Ils correspondent à la **protection de la bande métier 0–700 Hz**, pas à la nécessité d'une bande parfaite jusqu'aux fréquences de Nyquist 1 333 Hz (étage A) ou 1 000 Hz (final). Le contenu 700–1 000 Hz **n'est pas garanti**. Il faut vérifier l'atténuation globale de chaque tonalité repliée dans 0–700 Hz, y compris les images créées par l'interpolation B ; les critères par étage seuls ne suffisent pas à prouver le système complet.

**Recette de simulation reproductible :**
- Source nominale `fs=80000/3 Hz`, `fs_A=fs/10`, `fs_up=3*fs_A=8000 Hz`, `fs_out=fs_up/4=2000 Hz`.
- `scipy.signal.kaiserord(80, (1966.6666666667-700)/(fs/2))` et `scipy.signal.kaiserord(80, (1300-700)/(fs_up/2))` donnent chacun `beta=7.85726`; dimensionner ensuite FIR A 107 taps, cutoff `(700+1966.6666667)/2=1333.333333 Hz` et FIR B 69 taps, cutoff `1000 Hz`, via `signal.firwin(numtaps, cutoff, window=('kaiser', beta), fs=...)`.
- Simuler A avec `signal.upfirdn(h_A, signal_XYZ, up=1, down=10)`, puis B via `signal.resample_poly(sortie_A, up=3, down=4, window=h_B)`. `resample_poly` **compense le gain ×3** de l'interpolateur et centre les filtres dans son oracle ; **ne pas** recopier sa logique de padding/centrage à l'identique dans un firmware causal sans modéliser le délai.
- Exécuter `signal.freqz` sur chaque filtre et vérifier bandes de passage/arrêt, puis des séries sinusoïdales continues avec suppression des bords de trame et calcul de spectres/tonalités de sortie ; garder la version SciPy, les coefficients binaires et des vecteurs de référence contrôlés lors de R1.

**Résultats de ce calcul hôte à coefficients flottants :** sur un maillage dense de fréquences, erreur maximale absolue en bande de passage 0–700 Hz **≈0,001 dB** pour chaque étage ; amplitude maximale de stopband **−80,6 dB à partir de 1 966,667 Hz** (A), **−78,6 dB à partir de 1 300 Hz** (B), selon ce maillage et ces coefficients précis. Des essais numériques de sinusoïdes sur la chaîne complète, à source nominale et en excluant les transitoires de début/fin, ont montré environ **−87 dB à 1 300 Hz, −82 dB à 1 967 Hz et −81 dB à 2 000 Hz** rapportés à l'amplitude injectée (autres fréquences testées : 25, 500, 700, 1 333, 2 667, 3 000, 4 000 et 6 000 Hz). **Il ne s'agit ni d'une borne de pire cas pour toutes les fréquences, ni du rejet réel du capteur ou du filtrage quantifié ARM.** Les essais de 2 Hz exigent une fenêtre de durée suffisamment grande pour mesurer l'amplitude sans biais RMS lié à un nombre non entier de cycles.

**Coût arithmétique idéal de réalisation polyphasée (sans overhead, par seconde) :** A `(8000/3)*107 = 285 333 MAC/axe/s`; B `2000*(69/3)=46 000 MAC/axe/s` ; total **≈994 000 MAC/s sur trois axes**. Pour cet oracle linéaire, délai de groupe théorique des deux filtres `53/(80000/3) + 34/8000 ≈ 6,24 ms`, **avant** les effets de phase/centrage des grilles, du batching, du FIFO, des timestamps et de la mise en fichier. Les 176 coefficients (107+69), communs aux 3 axes, représentent **704 octets en float32** ou **352 octets en Q15** hors état ; aucun budget RAM final sans MAP, tailles de blocs et choix d'arithmétique.

**Attention implémentation et métier :**
- La voie de calcul flottante est une **référence de filtrage**, pas le choix imposé du produit. Comparer ensuite une version embarquée `float32` et/ou `Q15/Q31`, accumulations 64 bits ou saturées appropriées, bornes sur le gain, arrondis, overshoot à pleine échelle et propagation d'erreurs. Les versions CMSIS-DSP **fast Q15** à accumulateur 32 bits peuvent déborder ; ne pas les adopter sans preuve de marge. Le signal à ±8 g est représenté par les comptes int16 signés du capteur, **pas par des milligrammes entiers int16**.
- `3/40` est le ratio nominal du capteur ; avec `26 667 Hz` effectifs au lieu de `80 000/3`, la cadence de fichier deviendrait **2 000,025 Hz**, soit **12,5 ppm** de différence. Une dérive plus élevée est possible. Soit l'on accepte une cadence exacte **dans l'horloge du capteur** en archivant `fs_out_effective` et le temps événementiel relié au temps de référence, soit on ajoute un resampling piloté par horloge qualifiée ; **pas d'étiquette « exactement 2 000 Hz réels » sans mesure**.
- Le flux de sortie conserve le DC et peut présenter des délais/transitoires : documenter échantillons initiaux non stabilisés, premier index et horodatage causal, absence d'interpolation cachée au travers d'une lacune FIFO, et offsets/phase réels pour reconstruction PC.
- **Critères proposés à confirmer en R1 avant la qualification :** gain DC dans ±0,5 % ; déviation d'amplitude du traitement numérique seul ≤0,1 dB de 2 à 25 Hz et ≤0,5 dB de 25 à 700 Hz ; contribution de tonalités repliées **dans 0–700 Hz** sous −60 dB pour des tonalités hors bande ≥1 300 Hz, avec tests d'alias et de quantification ; politique des fréquences 700–1 300 Hz explicitement déclarée non qualifiée ; compte, continuité, horloge et anomalies vérifiés sans compensation de pertes. Les mesures en chaîne complète (filtre IIS3DWB, fixation, amplitude ±8 g, bruit, CEM) restent une qualification physique distincte.

**Références indépendantes :** [SciPy `resample_poly`](https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.resample_poly.html), [CMSIS-DSP FIR decimation](https://arm-software.github.io/CMSIS-DSP/latest/group__FIR__decimate.html), [CMSIS-DSP FIR interpolation](https://arm-software.github.io/CMSIS-DSP/main/group__FIR__Interpolate.html), [ST AN5444 Rev3](https://www.st.com/resource/en/application_note/dm00683267-iis3dwb-ultrawide-bandwidth-lownoise-3axis-digital-vibration-sensor-stmicroelectronics.pdf).

## 4. Branches archive et LIVE : priorité et absence de double acquisition

- **Un seul propriétaire physique FIFO SPI3** : décodage/temps commun, puis consommateur archive prioritaire et consommateur LIVE dérivé ; pas deux lecteurs indépendants qui perturbent les tags/indices.
- Le code `platform/stm32/live_fifo_acquisition.c` permet un consommateur externe ; `fifo_event_time.c` restitue des XYZ datés ; `src/application/live_metrology.c` et `live_dynamic.c` calculent les métriques d'affichage. Le LIVE opérateur AC1 traite déjà la voie passe-haut expérimentale, mais **elle n'est pas physiquement qualifiée**, et le rapport Codex actif demeure `BLOCKED`.
- La fenêtre LIVE actuelle n'est **pas** un mécanisme d'archivage continu ; ne pas l'utiliser comme source du fichier en sautant les périodes sans métrique. La sortie archive reçoit **tous** les XYZ décodés valides via le ré-échantillonneur, quel que soit le rythme de publication LIVE.
- En surcharge, ralentir/différer en premier LIVE et maintenance non critique ; **jamais** faire taire une lacune d'archive. Toute attente SD, FAT, `sync` ou FRAM doit laisser le capteur être servi avant la limite démontrée.

## 5. Fichier microSD / PC — choix candidat, distinct des secteurs bulk historiques

### 5.1 Contenant PC

**Premier candidat : FAT32 + fichier(s) de campagne binaires auto-décrits et versionnés**, avec lecteur/importateur PC ; pas de CSV écrit en continu (beaucoup plus volumineux et coûteux). À 12 000 octets/s : **345 600 000 octets/8 h** et **1 036 800 000 octets/24 h**, **hors en-têtes, CRC, métadonnées et filesystem** ; 57 600 000 XYZ/8 h et 172 800 000 XYZ/24 h. Le plafond d'un fichier FAT32 (~4 Gio moins 1 octet) ne contraint donc pas une campagne 24h à ce débit, même avec une marge raisonnable pour les blocs. Prévoir arrêt sûr avant retrait, fichiers lisibles directement par l'OS et un import PC capable d'en vérifier le contenu.

L'interface `CampaignBulkMedia` actuelle est **byte-addressed RAW**, pas une abstraction de fichiers FAT32 ; ses blocs v1 ont **32 octets de header, CRC, `TR2_CAMPAIGN_BULK_SAMPLE_RECORD_SIZE=16`**. **Ne pas réinterpréter silencieusement les blocs déjà encodés**. Réutiliser les idées éprouvées (CRC32, indices, progression monotone, double checkpoint, recovery), créer explicitement un format d'**archive XYZ compact version 2 candidat** et une adaptation logique du backend FS. Le modèle `CampaignDataStoreBulk` existant bloque son `flush` si les tailles ne sont pas multiples de 16 : une adaptation ciblée/versionnée est donc indispensable ; ne pas brancher les 6 octets directement sur ce contrat.

### 5.2 Proposition de format logique, **pas encore mise en octets**

**En-tête de campagne versionné** : magic/version, identité du TR2 et numéro de campagne/run, configuration/axes et orientation/site, capteur/firmware, ±8g réellement relu, sensibilité et unités des entiers, ODR source nominal/mesuré, période/grille output, filtre (identifiant/version/coefficients ou empreinte et réponse qualifiée), temps de début/validité UTC et référence monotone, règles de saturation/pertes, état de clôture et options du fichier.

**Blocs à taille bornée, de préférence alignés au secteur ou 4 Kio** : numéro de bloc, premier index XYZ en sortie (uint64), nombre réel d'échantillons, référence temporelle/segment, payload **XYZ en int16 little-endian X,Y,Z entrelacés = 6 octets/XYZ**, flags de qualité/erreur, CRC32 de bloc, padding explicite en dernier bloc. Les unités int16 sont des **comptes capteur ré-échantillonnés**, interprétés via la sensibilité/configuration et une règle d'arrondi/saturation définie. Ne pas convertir chaque sample en int16 *milligrammes* : à ±8g, on gaspillerait la précision inutilement. Le `int32 mg` conserve son rôle pour certains calculs/API, pas comme payload 6B. En cas de changement de FS pendant la campagne, segmenter ou rejeter, ne jamais mélanger deux échelles sans metadata.

**Pied de fichier / état durable** : dernier préfixe confirmé, compte échantillons/blocs/segments, anomalies, checksum(s) et état `COMPLETE` seulement après STOP, drainage, écritures et barrière de durabilité effectivement satisfaits. Une campagne interrompue est `INCOMPLETE/RECOVERED` avec son préfixe validé, **pas** « 8 heures complètes ». Une empreinte SHA du fichier complet peut être calculée sur PC ; ne pas imposer SHA temps réel sur chaque échantillon dans le microcontrôleur.

**Précision de format** : taille des headers, règle CRC, fréquence numérateur/dénominateur, encodage d'horloge, machine d'états et extensibilité restent **à geler après tests hôte**, non décidés par cette note. Le fichier doit être indépendant du fait qu'il soit copié manuellement depuis FAT32 ou téléchargé en V1.5.

### 5.3 Durabilité et interruptions d'alimentation

- Initialisation et éventuelle **préallocation** du fichier à STOP/avant START, si le driver FAT et le média la supportent sans bloquer une acquisition déjà démarrée ; la fonction `f_expand` de FatFs est un **candidat à auditer**, pas un résultat expérimental. Un fichier préalloué ne vaut pas « données valides » : le préfixe validé doit rester délimité séparément.
- File RAM bornée entre DSP et writer (double-buffer ou ring **dimensionné par MAP et latences physiques mesurées**). Transfert SPI2 par machine d'état/IRQ/DMA si les pauses de carte l'exigent ; capteur SPI3 prioritaire. 1 s de carte indisponible = 12 ko produits, 5 s = 60 ko, sans marge. La moyenne 12 ko/s ne protège pas d'une pause longue.
- Ordre de publication : écrire données + CRC -> établir la barrière de durabilité disponible -> publier checkpoint atomique/recoverable avec sa propre vérification -> seulement alors avancer le préfixe revendiqué durable. Sur FAT32, la stabilité du répertoire/allocation/FAT après coupure doit être **qualifiée séparément** : `f_sync` n'est pas une promesse de survie à tout brownout ni une validation de l'électronique microSD.
- Réutiliser les notions d'invariants `campaign_bulk_checkpoint` / `CampaignDataStoreBulk`, mais **ne pas réserver sans audit une zone FRAM supplémentaire** ni mélanger checkpoint de campagne et autorité transactionnelle B5 existante. Évaluer un petit journal de checkpoint lié au fichier sur la microSD et la capacité de reconstruction PC par scan CRC/indices, avec essais de coupure représentatifs.
- Aucune écriture silencieuse après `STOP/SAFE_REMOVE`; après retrait/reboot/média plein/timeout SD/queue saturée, publier des indicateurs de défaut et conserver la preuve du dernier préfixe vérifié. Ne pas réclamer une campagne 8h réussie avec pertes cachées.

**Référence FS :** [FatFs documentation officielle, interface disk_* et limites](https://elm-chan.org/fsw/ff/doc/appnote.html) ; [f_expand et contraintes de préallocation](https://elm-chan.org/fsw/ff/doc/expand.html). Choix et provenance/version/licence de la bibliothèque à auditer selon gouvernance ; pas d'ajout implicite de dépendance à ce stade.

## 6. Contrat minimum du lecteur PC

Lire la carte depuis un lecteur SD standard, reconnaître le format/version, présenter capteur/site/orientation/FS, plage de temps, total XYZ et **fréquence effective**. Vérifier tous les CRC et indices, refuser les formats inconnus, distinguer campagne complète/incomplète et segments dégradés, signaler trous, saturation, unités/horloge incertaines. Exporter si nécessaire CSV/Parquet avec timestamps reconstruits, calculer bandes 2–25 / 2–700 et indicateurs **a posteriori**, sans écraser le binaire de référence. Un fichier lisible par Windows mais incompréhensible/incorrect après import **n'est pas un succès de V1**.

## 7. Plan de preuves et critères de passage

| Gate | Preuve nécessaire | Pas encore démontré |
|---|---|---|
| R1-DSP-A | Oracle hôte déterministe, réponse 2–25 puis 2–700 Hz, gain DC, alias, phase/délai, transitoires, tolérances explicites | Tout filtre ré-échantillonneur 2k qualifié |
| R1-DSP-B | Chaîne XYZ native -> blocs compacts et import identique, sans données fabriquées ; anomalies/horloge injectées | Format v2 / lecteur PC |
| R1-ARM | MAP/BSS/stack et temps calcul par sample sur image ciblée ; marge FIFO/IRQ démontrée | CPU/DMA/SRAM intégrés |
| R2-SPI2 | TR2-002 identifié, microSD 32 Go identifiée, lecture/écriture/readback/latences en banc seul | Driver SD SPI |
| R2-COEX | FIFO natif + traitement + SPI2 + SPI1 + RTU, 12 ko/s, aucun overflow/perte silencieuse, backlog borné, qualité vérifiée | Temps réel avec stockage |
| R3-8H | 8h vraies mesures XYZ, card extraction/import PC, CRC/indices sans anomalie non signalée, coupure/STOP/retirement testés | GO produit V1 |
| R3-24H | 24h si 8h satisfaites, endurance et vieillissement | Option secondaire |

Les tolérances quantitatives métrologiques et d'horloge (gain/ripple, rejet d'alias, phase, dérive en ppm, latence maximale de service FIFO et perte durable acceptable après coupure) restent à **proposer et contrôler avant chaque gate concerné**, sans inventer une qualification au stade R0.

## 8. Invariants Git, matériel et missions

- **Point de retour R0 explicitement demandé et vérifié :** `a3f5ac33a728a5afd028192654c17e5b04ee8d7b` ; les commits documentaires postérieurs n'en déplacent pas le contenu. Tag annoté de sauvegarde d'état **à créer et vérifier plus tard**, distinct des tags industriels `tr2/Mnnn`, **sans `reset --hard` / `push --force`**. Un commit Git ne sauvegarde ni firmware flash ni FRAM ni médias/bases Windows externes.
- **TR2-002** = seconde NUCLEO historique, microSD 32 Go libre d'essais en écriture, banc non monté ; **TR2-001 intouché** pour ces expérimentations. Identités ST-LINK/UID/microSD à relire avant tout essai ; manipulations physiques à l'utilisateur.
- Mission active `TR2-20261028-ACCEL-FS8G-VIBRATION-AC`, contrat `READY`, dernier rapport `BLOCKED` sur paquet Windows AC1, **non clôturée**. **Aucune mission Codex nouvelle, aucun flash/debug, aucune modification firmware ni qualification** par cette note. Le futur contrat R1/R2 doit être publié par ChatGPT uniquement après vérification et traitement gouverné du contrat bloqué, avec les permissions et garde-fous `AGENTS.md`.

## 9. Sources exactes du dépôt

- [Cahier V1 R0](R0_METIER_20261029_CAHIER_DES_CHARGES.md) et [matrice des écarts](R0_METIER_20261029_MATRICE_ECARTS.md).
- [Étude SPI2 et ressources ARM](R0_METIER_20261029_ETUDE_MICROSD_SPI2.md).
- `../05_Firmware/platform/stm32/live_fifo_acquisition.c`, `fifo_event_time.c`, `stm32_r4u_runtime.c`, `main.c`.
- `../05_Firmware/src/application/live_metrology.c`, `live_dynamic.c`, `live_runtime.c`.
- `../05_Firmware/include/tr2/persistence/campaign_bulk_block.h`, `campaign_bulk_media.h`; `../05_Firmware/src/persistence/campaign_data_store_bulk.c`, `campaign_bulk_checkpoint.c`.
- [AN5444](https://www.st.com/resource/en/application_note/an5444-iis3dwb-ultrawide-bandwidth-lownoise-3axis-digital-vibration-sensor-stmicroelectronics.pdf) sections 3 (filters), 6 (FIFO, tags, timestamps), [FatFs](https://elm-chan.org/fsw/ff/doc/appnote.html).
