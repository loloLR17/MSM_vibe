# MSM TR2 — R0-MÉTIER — Plan d'essais déterministes R1-DSP (XYZ 26,667 kHz -> 2 kHz)

**Statut : protocole de qualification PROPOSÉ / NON EXÉCUTÉ par Codex / NON GELÉ.** Date : 29/10/2026. Auteur : ChatGPT (pilotage R0). **Objet :** créer des oracles reproductibles, mesurer les erreurs DSP et interdire l'implémentation d'un filtre dont la réponse complète est incorrecte. Cette note **n'est pas une mission exécutable** et ne donne aucune autorité de flash/debug.

Références : [chaîne de données R0](R0_METIER_20261029_CHAINE_DONNEES_ARCHIVE_XYZ.md), [cahier métier V1](R0_METIER_20261029_CAHIER_DES_CHARGES.md), [étude matériel SPI2](R0_METIER_20261029_ETUDE_MICROSD_SPI2.md), [roadmap R0–R6](R0_METIER_20261029_ROADMAP.md). Source de vérité `loloLR17/MSM_vibe/main` ; HEAD inspecté avant rédaction `731f514e89a6b7fb85b56cf9be0ba5c94d9c2b3f`.

## 1. Objectifs et conditions de passage

1. **Archive prioritaire :** préserver DC/gravité et XYZ signés, conserver tous les échantillons numériques produits après ré-échantillonnage sous réserve des segments et anomalies explicitement marqués.
2. **Métrologie numérique seulement :** cœur 2–25 Hz, bande 2–700 Hz, cadence cible 2 000 échantillons/s/axe, aucune perte silencieuse, indices/temps reconstructibles. Séparer le niveau de qualification *filtre numérique flottant*, *filtre numérique ARM/quantifié* et *chaîne physique capteur+montage*.
3. **Critères candidat à geler avant R1 :** gain DC ±0,5 %, erreur gain ≤0,1 dB de 2–25 Hz et ≤0,5 dB de 25–700 Hz ; contribution de toute tonalité d'entrée ≥1 300 Hz dans **0–700 Hz en sortie** au maximum **−60 dB** en amplitude relative à l'excitation, en tenant compte du repliement et des erreurs numériques. Les plages **700–1 300 Hz** de l'entrée/sortie ne sont pas promises comme bande utile ; tester leur comportement mais ne pas les qualifier arbitrairement.
4. **Admission streaming :** résultats strictement indépendants du découpage des appels à l'API, du nombre de blocs et de la taille des tampons ; comportement de début/fin/panne explicite.
5. **Pas de réussite extrapolée :** vérifier sur l'oracle *entier* de la chaîne, pas seulement les réponses fréquentielles de chaque FIR. Les mesures sous vibrations réelles et 8 heures de SD appartiennent aux portes ultérieures.

## 2. Constat indépendant : première conception rejetée, variante à éprouver

Recalcul effectué localement par ChatGPT avec **Python / NumPy 2.3.5 / SciPy 1.17.0**, synthèse de sinusoïdes complexes de durée 2 secondes (section centrale pour exclusion des transitoires) et filtres `float64` :

| Entrée | FIR A107 + B69 initial | FIR **A225 + B69** candidat |
|---|---:|---:|
| 700 Hz | ≈0 dB | ≈0 dB |
| 1 300 Hz | −87,50 dB | −165,63 dB |
| 1 400 Hz | −64,30 dB | −141,46 dB |
| **1 500 Hz** | **−38,54 dB — NOK** | **−120,02 dB** |
| **1 600 Hz** | **−30,26 dB — NOK** | **−107,92 dB** |
| 1 700 Hz | −30,86 dB | −97,75 dB |
| 1 966,67 Hz | −82,22 dB | −90,97 dB |
| 2 000 Hz | −81,37 dB | −88,71 dB |

Le premier filtre passe-bas 107 taps était dimensionné pour un arrêt à **1 966,667 Hz** ; combiné à la transition de B, il laissait passer des repliements à 1 500–1 700 Hz. **Ne pas implémenter ce candidat.** Avec A225, le stopband A débute à **1 300 Hz**. Sur un balayage exploratoire de **605 fréquences entre 1 300 et 13 200 Hz** (pas 25 Hz avec un pas plus serré 5 Hz vers 1 300–2 100 Hz), le pire relevé est **−87,85 dB à 1 880 Hz**. **Ce n'est pas un pire cas mathématiquement prouvé** ni un résultat embarqué ; les oracles devront étendre le balayage et rechercher d'éventuels pics étroits.

## 3. Paramètres explicites de l'oracle flottant *causal*

| Élément | Valeur candidat |
|---|---|
| `f_in` nominal | `80000/3 Hz` (26 666,666… Hz) |
| A, décimation | `D=10`, FIR 225 taps, fenêtre Kaiser `beta=7.85726`, cutoff `1000 Hz`, fréquence de design `80000/3 Hz` |
| `f_A` | `8000/3 Hz` |
| B, ratio rationnel | interpolation `L=3` -> `8000 Hz`, FIR 69 taps Kaiser même beta et cutoff 1000 Hz ; décimation `M=4` -> 2000 Hz |
| Gain d'interpolation | **×3** appliqué au filtre B dans une réalisation `upfirdn` causale ; ne pas oublier |
| Phase et index | A émet au sample source `n=0,10,20,...` ; B émet aux indices de grille interpolée `q=0,4,8,...`. Même origine pour les trois axes. |
| Padding et fin | Pré-histoire initiale nulle **déclarée**, pas de queue finale émise automatiquement en campagne. Un éventuel `flush` de fin doit être **explicite et étiqueté** : la queue calculée avec zéros n'est pas une nouvelle mesure capteur. |
| Temps | Délai de groupe théorique `112/(80000/3)+34/8000=0.00845 s`. Événements et timestamps causaux/compensés documentés, sans attribuer les 8,45 ms de démarrage à des données réellement captées avant START. |
| Quantification | Oracle float64 -> `np.rint` (ties-to-even) -> saturation int16 ; **uniquement pour fabriquer les oracles de comparaison**, pas décision de l'arithmétique STM32 finale. |

Pour éviter l'oracle incorrect précédent, **ne pas** combiner `upfirdn` A (qui produit une queue) et `resample_poly` B (qui recentre/compense la phase) sans règles de recadrage : cela modifie artificiellement le nombre et l'alignement des échantillons.

### 3.1 Recette autonome de génération des vecteurs simples

```python
import hashlib
import numpy as np
from scipy import signal

FS = 80000.0 / 3.0
BETA = 7.85726
H_A = signal.firwin(225, 1000.0, fs=FS,
                    window=("kaiser", BETA))
H_B = signal.firwin(69, 1000.0, fs=8000.0,
                    window=("kaiser", BETA))

def causal_axis(x):
    """Un segment source; phase A n%10=0, B q%4=0, préhistoire nulle."""
    x = np.asarray(x, dtype=np.float64)
    a = signal.upfirdn(H_A, x, up=1, down=10)
    a = a[:(len(x) + 9) // 10]  # PAS de queue non mesurée
    y = signal.upfirdn(3.0 * H_B, a, up=3, down=4)
    return y[:(3 * len(a) + 3) // 4]

def encode_xyz(xyz):
    y = np.column_stack([causal_axis(xyz[:, axis])
                         for axis in range(3)])
    q = np.clip(np.rint(y), -32768, 32767).astype("<i2")
    return q, hashlib.sha256(q.tobytes()).hexdigest()

N = 160000  # 6 s exactes au nominal (80000/3 Hz)
dc = np.empty((N, 3), dtype=np.int16)
dc[:, 0], dc[:, 1], dc[:, 2] = 16384, -8192, 4096
q, sha256 = encode_xyz(dc)
assert q.shape == (12000, 3)
assert (q[100:1000] == [16384, -8192, 4096]).all()
print("DC", sha256)

imp = np.zeros((N, 3), dtype=np.int16)
imp[0, 0] = 16384
q, sha256 = encode_xyz(imp)
assert q.shape == (12000, 3)
print("IMPULSE", sha256)
```

**Résultats informatifs de la reproduction locale** (NumPy 2.3.5 / SciPy 1.17.0, filtre float64, arrondi ties-to-even, little endian, 6 s, 12 000 jeux XYZ) :

| Vecteur | SHA256 des 72 000 octets XYZ int16 entrelacés |
|---|---|
| DC constant `[16384,-8192,4096]` | `af812b298f2d749bdb75170e53d838dd5dd69e0d827c06d73d0385d8eece37c0` |
| Impulsion au premier jeu `X=16384; Y=Z=0` | `0350a5091e4e2cf8f2f0e8d00a7637fb132212f68d127e8361f33352aa0d2971` |

**Limite essentielle :** ces empreintes sont des **témoins de reproductibilité d'un oracle local**, pas des empreintes **normatives** du firmware en `float32` ou Q15/Q31. Des coefficients arrondis, une autre gestion des transitoires, une autre implémentation polyphasée ou une différence de précision FPU peuvent changer quelques comptes ; fixer les marges de tolérance avant de qualifier chaque variante. La comparaison octet pour octet n'est obligatoire qu'entre exécutions de **la même** implémentation/phase/quantification déterministe.

## 4. Matrice des essais à générer

| Identifiant | Stimulus déterministe | Vérification fondamentale |
|---|---|---|
| **DSP-00** | Entrée nulle sur 3 axes, 6 s | 12 000 sorties nulles, état initial/final documenté, zéro NaN/overflow |
| **DSP-01** | DC constant `X=16384,Y=-8192,Z=4096`, 6 s | gain DC, transitoire initial et SHA témoin ; plateau XYZ exact en oracle |
| **DSP-02** | Impulsion `X[0]=16384`, Y/Z nuls, 6 s | forme de réponse, délai/phase de grille, aucune diaphonie entre axes |
| **DSP-03** | Échelon à `t=3s`, `X=+12000`, `Y=-12000`, `Z=0`, jusqu'à 6 s | dynamique, stabilité, dépassements, évolution aux limites des blocs |
| **DSP-04** | Sinusoïdes **2, 5, 25, 100, 333⅓, 666⅔, 700 Hz**, sur 6 s, amplitude 12 000 comptes, phases déterministes (0 et π/7) | gain utile, phase/retard et réponse proche bord 700 Hz ; 2 Hz mesuré sur ≥12 périodes |
| **DSP-05** | Sinusoïdes **800, 1 000, 1 300, 1 400, 1 500, 1 600, 1 700, 1 880, 1 966⅔, 2 000, 3 000, 6 000, 12 000 Hz**, amplitude 12 000 comptes | filtrage et contamination repliée dans 0–700 Hz, en incluant les **cas A107 historiquement fautifs** |
| **DSP-06** | Balayage tonal entre 1 300 Hz et Nyquist source, au minimum pas 25 Hz ; pas ≤5 Hz de 1 300 à 2 100 Hz ; phases sinus ET cosinus | pire cas observé du rejet, recherche pics étroits ; pas de faux « globalement OK » sur seules tonalités faciles |
| **DSP-07** | XYZ multiton DC+25+700 Hz et perturbation 1 600 Hz (somme sans écrêtage), phasages fixes | linéarité, non-régression gain et fuite alias en présence de DC |
| **DSP-08** | Niveaux zéro, ±1, presque pleine échelle et extrêmes int16, inversions brutales ; calcul float32 / Q15/Q31 candidats | arrondis, clipping/saturation signalé, pas de débordement ou wrap silencieux |
| **DSP-09** | Injecter un missing sample/tag, overrun FIFO, rupture d'index, ancre temps incohérente, redémarrage d'acquisition | erreur explicite, segment fermé/invalide ou reprise délimitée, **aucune interpolation de données imaginaires** |
| **DSP-10** | Rejouer une séquence identique en chunks `1, 7, 10, 13, 31, 255, 512, 1 024` et partitions pseudorandom à graine fixe | mêmes 12 000 sorties, même ordre/indices/CRC à configuration et arithmétique identiques |
| **DSP-11** | Horloges nominale, dérive ±50 ppm, ±1 000 ppm, ancrages UTC invalides, rollover 32 bits et reset du capteur | `fs_out_effective` et segments bien identifiés ; ne jamais afficher exactement 2000 Hz réels sans preuve |
| **DSP-12** | Démarrage, STOP avant remplissage du premier FIR, STOP en milieu de phase ×3/4, troncature, flush explicite | compte et timestamps exacts, transitoires/queue distingués des échantillons mesurés |
| **DSP-13** | Cadence nominale, CPU/Modbus simulés, variations de chunk, forte activité FRAM et longues latences SD injectées | aucun travail numérique qui masque un retard FIFO ; file bornée, alarmes de dépassement, traitement secondaire différable |
| **DSP-14** | Sérialisation int16 X,Y,Z little-endian et métadonnées de fréquence/FS/état avec CRC, puis import PC | compte, unités, signe, axes et numérotation identiques de bout en bout, version explicite |

La génération des vecteurs réels **int16** doit préciser le générateur : `x[n] = clip_int16(round_nearest_even(A * sin(2π f n / fs + φ) + DC))`, l'ordre des axes, `fs` rationnel exact, unités « comptes bruts », la durée/indices source et les graines aléatoires. Tester aussi les paires sinus/cosinus pour éviter de conclure à une bonne atténuation sur une fréquence particulière où la phase annulerait artificiellement le résidu.

## 5. Comparaisons et seuils de validation

### 5.1 Couche oracle numérique hors cible

- Vérifier indépendamment **chaque FIR**, puis **la cascade complète** (nombre/index/time origin/phase), ainsi que les réponses fréquentielles en bande utile et la bande de contamination dans **0–700 Hz** pour toutes les entrées ≥1 300 Hz.
- En R0, les valeurs publiées sont **observées** ; la validation d'une exigence universelle demande une justification de pire cas (bornage analytique ou recherche dense et marges conservatrices), pas une fréquence par dizaine de hertz.
- Ne pas intégrer un filtre dont une fuite de rejet est déjà démontrée : **A107 est NOK**. A225+B69 est **candidat**, non QUALIFIED.

### 5.2 Couche streaming hôte

- Calcul causal de référence échantillon par échantillon, puis traitement par chunks. **Octets identiques** à ceux du même oracle causal dans la même arithmétique, quelle que soit la partition de chunks.
- Comparaison oracle `float64` -> portage `float32/Q` avec **tolérance numérique explicite en comptes**, à établir pour DC, utile, bruit et pleine échelle. Évaluer pré-saturation interne et accumulation, gain de quantification, décalage d'état et pire écart, et non un RMS d'erreur seul qui dissimule des rares erreurs graves.
- Critères fréquence & alias du §1 applicables au **résultat porté**, pas simplement au filtrage offline. Sous erreur de capteur/horloge, suspendre l'affirmation de validité au lieu de masquer les trous.

### 5.3 Couche ARM (avant coexistence microSD)

- Produire manifestes reproductibles de toolchain/flags/coefficients/empreinte de l'ELF et image/identité du banc au moment des essais, rapport des sections FLASH/BSS/data, pile, allocations DMA, consommation et accès mémoire.
- Mesurer cycles DWT et distributions (moyenne, P95/P99, max) **par bloc et par échantillon source**, comparées au temps réel disponible au débit natif. L'objectif est de prouver une borne avec la lecture FIFO en charge, pas de convertir `~1,94 M MAC/s` en un pourcentage CPU supposé.
- Après qualification isolée, soumettre au service FIFO natif, aux IRQ LPUART1/TIM6 et aux appels FRAM bornés, puis aux écritures SD. Relever pertes, anomalies de temps, backlog RAM, coûts des « busy » microSD. Le nombre idéal **19,2 ms** de capacité FIFO complète n'est **pas** une autorisation de bloquer le CPU 19 ms.

## 6. Critères de sortie du lot R1-DSP

**PASS DSP numérique** seulement si : code oracle et versions publiés ; filtres/coefficients/phase identifiés ; tableaux de tous les tests et cas limites conservés ; réponse de la **chaîne complète** conforme aux tolérances stabilisées ; fichiers témoins SHA/CRC reproduits si pertinents ; streaming indépendant du chunk ; aucune perte masquée ; CPU/RAM STM32 qualifiés séparément pour un build précis.

**BLOCKED/NOK** si : tolérances non atteintes, garde-fous absents, variation de nombre d'échantillons inexpliquée, repliement non testé, overflow caché ou preuve de continuité insuffisante. Une erreur filtrage ne doit pas devenir un faux problème SD et réciproquement.

**Périmètre R0 actuel :** publication d'un protocole et de calculs **hors cible uniquement**, sans création/écrasement du contrat Codex. Le contrat `TR2-20261028-ACCEL-FS8G-VIBRATION-AC` reste `READY` avec rapport `BLOCKED`; la transition de mission doit être gouvernée avant R1 ou R2 effectif. **Point de retour Git impératif** `a3f5ac33a728a5afd028192654c17e5b04ee8d7b`, à taguer explicitement et vérifier à distance avant expérimentations techniques. TR2-002 seul banc futur, TR2-001 intégralement préservé.

## 7. Sources complémentaires

- [SciPy `upfirdn`](https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.upfirdn.html) et [SciPy `firwin`](https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.firwin.html).
- [CMSIS-DSP decimator FIR](https://arm-software.github.io/CMSIS-DSP/latest/group__FIR__decimate.html) et [interpolator FIR](https://arm-software.github.io/CMSIS-DSP/main/group__FIR__Interpolate.html) : numTaps interpolation divisible par facteur, accumulateur Q15 fast susceptible d'overflow, vérifier APIs/versions effectivement intégrées.
- [Étude temporelle R4 existante](../05_Firmware/LIVE_R4.md), [dernier rapport Codex](Echanges_Codex/DERNIER_RAPPORT.md).
