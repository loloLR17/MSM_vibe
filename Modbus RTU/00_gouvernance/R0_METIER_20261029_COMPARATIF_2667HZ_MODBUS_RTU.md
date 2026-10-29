# MSM TR2 — R0-MÉTIER — Comparaison 2 666,7 / 2 000 Hz et possibilité de flux XYZ sur Modbus RTU

**Statut : ÉTUDE DE CONCEPTION, NOUVELLE OPTION PRIVILÉGIÉE SOUS RÉSERVE DE QUALIFICATION, PAS D'ARBITRAGE NORMATIF NI D'IMPLÉMENTATION.** 29/10/2026, ChatGPT. Référence distante vérifiée au démarrage : `loloLR17/MSM_vibe/main` HEAD `23f5ddc28693dfb6bae63845f054aca58edf4bd9`. Demande utilisateur : comparer le maintien de l'étage intermédiaire de décimation à environ 2 666,7 Hz (économie de polyphasage) et étudier **par principe, en ordre de grandeur**, l'envoi de l'intégralité des données de vibration via Modbus RTU.

Cette note ne remplace **pas** le cahier métier préexistant qui définit toujours 2 000 Hz comme cible validée, tant que le choix de 2 666,7 Hz n'est pas formellement arbitré ; elle documente toutefois une **alternative sérieuse recommandée à la comparaison R1**.

## 1. Hypothèses : les deux sorties conservent XYZ et le DC

- IIS3DWB à ODR native **nominale** `80 000/3 = 26 666,666… Hz`, à mesurer réellement et non à supposer exacte ; trois axes signés de 16 bits (6 octets par triplet XYZ), après un filtre numérique de passe-bas qui **ne retire pas la pesanteur**.
- Bande utile métier prioritaire 2–25 Hz, étendue cible 2–700 Hz. Ce ne sont pas des exigences de fidélité à 1 000 Hz ; un taux de sortie plus élevé est permis si ses coûts/bénéfices sont documentés.
- **2 000 Hz :** FIR A225 puis FIR B69 en ×3/4, selon [étude de filtrage R0](R0_METIER_20261029_CHAINE_DONNEES_ARCHIVE_XYZ.md). **2 666,666… Hz :** **FIR A107 seul**, passe-bas à `1333,333 Hz`, décimation **/10** ; pas d'étage B.
- Aucune de ces voies n'est encore **physiquement ni numériquement qualifiée sur ARM** ; les coûts MAC/s ci-dessous sont des ordres de grandeur mathématiques d'implémentations polyphasées efficaces, pas des mesures de cycles STM32.

## 2. Comparaison fonctionnelle et temporelle

| Paramètre | 2 000 Hz, A225+B69 | 2 666,7 Hz, A107 /10 |
|---|---:|---:|
| Facteur de fréquence source/output nominal | 40/3 | 10 |
| Nyquist sortie | 1 000 Hz | 1 333,33 Hz |
| Marge Nyquist au-dessus de 700 Hz | 300 Hz | 633,33 Hz |
| FIR/complexité | 225 et 69 coefficients + phases polyphasées ×3/4 | 107 coefficients + décimation simple |
| Coût idéal XYZ | **1 938 000 MAC/s** | **856 000 MAC/s** |
| Gain sur ce coût | référence | **−55,83 %** |
| Débit payload XYZ 6 octets/échantillon | **12 000 B/s** | **16 000 B/s** |
| Volume 8 h, sans overhead | **345,6 Mo** | **460,8 Mo** |
| Volume 24 h, sans overhead | **1,0368 Go** | **1,3824 Go** |
| Délai de groupe FIR théorique | **8,45 ms** | **53/(80 000/3) ≈1,99 ms** |
| Cadence exacte réelle | `f_sensor × 3/40`, à estimer | `f_sensor / 10`, à estimer |

**Observation indépendante, calcul local de la réponse A107 seule** : `scipy.signal.firwin(107, 1333.3333333, window=('kaiser', 7.85726), fs=80000/3)`, `freqz` (1 048 576 points), gain DC normalisé 1. Erreur amplitude maximale en bande 0–700 Hz environ **0,001 dB**, pire réponse calculée pour `f >= f_out-700 = 1966,666… Hz` environ **−80,60 dB**. Cela rend A107 **candidat crédible** pour préserver 0–700 Hz après **une seule décimation /10**. La fuite de −30 dB constatée pour A107+B69 à 2 000 Hz provenait de la **cascade et de son second repliement**, **pas** d'un défaut de cet A107 employé seul pour 2 666,7 Hz. L'analyse des alias à toutes les fréquences, coefficients quantifiés, transitoires et acquisition physique reste à qualifier. La zone 700–1 333 Hz en sortie n'est **pas** contractuellement une bande à fidélité garantie.

**Contrepartie :** la carte SD reçoit **33,3 % de données en plus** et doit absorber 16 ko/s contre 12 ko/s, toujours très inférieur aux débits nominaux d'une carte microSD ; l'obstacle reste les pauses internes, la latence et la coexistence FIFO. Sur une carte 32 Go, 460,8 Mo/8h n'est pas une contrainte de capacité déterminante, mais l'endurance 8h et l'intégrité restent à prouver.

**Proposition R0 :** faire du **/10 à 2 666,7 Hz le premier essai DSP** avant toute adaptation ×3/4 ; ne modifier la cible métier 2 000 Hz qu'après confirmation utilisateur de ce choix, sans modifier silencieusement les spécifications héritées. Si les tests temporels ou spectraux invalident A107, rechercher le correctif **minimal pour /10**, et non réintroduire automatiquement un second étage.

## 3. Modbus RTU : hypothèses vérifiées dans le firmware réel

- `../05_Firmware/platform/stm32/stm32_serial_transport.c` : `BaudRate=115200`, `WordLength=9B` avec parité EVEN = **8E1** utile, un start/8 data/une parité/un stop = **11 bits par octet** ; TX buffer 256 octets, RX par interruption ; LPUART1/TIM6 IRQ priorité 5.
- `../05_Firmware/src/application/live_modbus_server.c` : FC03 lecture de **1 à 125 registres** (2 octets chacun), mais **aucun espace de registres bulk XYZ en continu**. Une transmission de fichier/stream serait un développement versionné nouveau, pas l'activation d'un registre existant.
- La spécification [Modbus Application Protocol V1.1b3, §6.3](https://www.modbus.org/file/secure/modbusprotocolspecification.pdf) définit 125 registres au maximum pour FC03. La [spécification Serial Line V1.02, §2.5.1.1](https://www.modbus.org/file/secure/modbusoverserial.pdf) recommande `t3.5=1,75ms` et `t1.5=0,75ms` en RTU au-dessus de 19 200 bauds. Le projet applique ces délais fixés au niveau du récepteur/temporisation UART.

### 3.1 Enveloppe de capacité théorique

**Modèle optimiste de tour complet, un seul maître et un seul capteur actif :** requête FC03 **8 octets RTU**, réponse **255 octets RTU** dont **250 octets data**, deux intervalles entre trames de **1,75 ms** ; `T=263×11/baud+2×0,00175` seconde par lecture, et `R_payload=250/T`. **Ne pas supposer** délais de traitement nuls sur carte réelle ; ajouter CRC/compteurs/indices/répétitions et supervision diminue le débit disponible. Échantillons 6B non nécessairement alignés avec les 250B de registres : un flux réel demandera fragmentation et metadata.

| Baud RTU hypothétique | Débit ligne brut max = baud/11 | Débit payload FC03 optimiste | Rapport demande 16 ko/s / capacité |
|---|---:|---:|---:|
| **115 200 — actuel** | **10,47 ko/s** | **8,74 ko/s** | **183 % : impossible** |
| 230 400 | 20,95 ko/s | 15,57 ko/s | 103 % : insuffisant même idéal |
| 460 800 | 41,89 ko/s | 25,57 ko/s | 63 % : possible en théorie pour un capteur |
| 921 600 | 83,78 ko/s | 37,66 ko/s | 42 % : plus de marge théorique |

À **2 kHz**, les données représentent 12 ko/s ; le débit Modbus **115 200 reste insuffisant** même au niveau physique, sans aucun overhead. Pour **2 666,7 Hz**, le minimum physique hors protocole serait **176 000 bit/s** (16 000 × 11), contre **132 000 bit/s** pour 2 kHz. Le fait qu'un adaptateur RS-485 accepte 460 800 ou 921 600 bauds ne valide **ni notre transceiver, ni le câblage de bord, ni les interruptions UART, ni notre implémentation RTU** à ces vitesses.

### 3.2 Autonomie, six capteurs et extraction a posteriori

- **Un seul capteur, streaming temps réel :** 460 800 bauds pourrait être étudié sur banc distinct mais utiliserait ~63 % de la capacité optimiste FC03, sans marge de garantie et au prix d'une forte activité UART/IRQ et de nombreux polls. Une déconnexion PC, panne du maître ou liaison défectueuse perd immédiatement le flux si la microSD n'enregistre pas parallèlement.
- **Six capteurs sur un même RTU partagé :** `6×16 000=96 000 B/s` rien qu'en XYZ ; **incompatible** avec 115 200 et 921 600 bauds, avant même les autres messages. Et Modbus RTU est un protocole **maître/interrogé**, pas un flux libre ou un bus multimaître.
- **Extraction d'un fichier de 8h enregistré à 2 666,7 Hz :** 460,8 Mo / 8,74 ko/s ≈ **14,65 h** à 115 200 ; /25,57 ko/s ≈ **5,01 h** à 460 800, *meilleurs cas non garantis*. Le retrait manuel de microSD reste beaucoup plus adapté à V1.
- **Alternative de principe :** pour un appareil câblé et supervisé en permanence, un flux UART/RS485 **non-Modbus ou protocolisé par blocs** pourrait employer autrement les silences et la longueur des trames, mais ce serait une **nouvelle interface distincte de la norme Modbus RTU V1** et n'apporterait pas l'autonomie exigée. Si un transfert réseau intégral devient utile, réserver l'étude à V1.5 **IP et téléchargement de fichier**, pas pousser artificiellement le flux dans les registres V1.

**Verdict :** **Modbus RTU n'est pas une absurdité technologique** pour quelques données bulk sur un appareil et une liaison rapide, mais **ne convient pas à notre besoin V1 de 6 capteurs autonomes, sans PC permanent, avec conservation 8 h minimum**. La microSD amovible reste indispensable ; Modbus conserve les commandes, diagnostics et indicateurs compacts.

## 4. Suites et garde-fous

1. R1-DSP : proposer et tester en premier **/10 A107, sortie 2 666,7 Hz** pour la bande 2–700 Hz et la fidélité DC, avec un oracle *causal*, tests d'alias à toutes fréquences, arithmétique embarquée, chronologie source et durée de campagne ; comparer au candidat 2 000 Hz seulement si un gain réel le justifie.
2. R2-SPI2 : dimensionner la file à **16 ko/s** si le candidat /10 est retenu, vérifier la pire latence/busy SD, la consommation SRAM et l'absence d'overflow FIFO. Le flux natif 26,667 kHz doit toujours être intégralement drainé.
3. V1 : **ne pas modifier** le protocole Modbus gelé ni démarrer un nouveau transport de fichiers sans décision ; traiter un streaming Modbus à haut débit comme une alternative théorique et non un nouveau prérequis.
4. **Versionnement :** conserver comme point de retour R0 demandé `a3f5ac33a728a5afd028192654c17e5b04ee8d7b`. Pas de tag nouveau ni de flash. TR2-002 libre pour futurs essais SPI2 après identification ; TR2-001 préservé.
5. **Échanges Codex :** `TR2-20261028-ACCEL-FS8G-VIBRATION-AC` demeure actif avec rapport `BLOCKED`. Aucune nouvelle mission ni évolution firmware dans cette étude ; une mission R1/R2 nécessite le traitement gouverné de la précédente.

**Nature des preuves :** chiffres de bande passante issus du protocole et du code actuel ; coût de FIR et fréquence comme modèles numériques, non résultats d'essais physiques, Ethernet et durabilité non qualifiés.

## 5. Vérification numérique indépendante du FIR /10 et risques (29/10/2026)

**Nature de la preuve : expérience numérique exécutée par ChatGPT hors dépôt/hors matériel, reproduite sous Python / NumPy 2.3.5 / SciPy 1.17.0.** Aucune compilation ARM, aucune mesure physique du STM32 ou de l'IIS3DWB, aucune publication de code DSP opérationnel. Les constats suivants portent exclusivement sur la sortie **~2 666,666… Hz** et le FIR A107 **sans second étage B**.

### 5.1 Coefficients et réponse fréquentielle

```python
import numpy as np
from scipy import signal
FS = 80000/3
BETA = 7.85726
H = signal.firwin(107, 1333.3333333333333,
                  fs=FS, window=("kaiser", BETA))
w, gain = signal.freqz(H, worN=2**20, fs=FS)
pb = 20*np.log10(abs(gain[w <= 700]))
sb = 20*np.log10(abs(gain[w >= FS/10 - 700]))
print(np.max(abs(pb)), np.max(sb))
# env. 0.00097964 dB, -80.60020 dB
```

- Gain DC `sum(H)=1,0000000000000002`, bande 0–700 Hz **max |erreur gain| 0,00097964 dB**, stopband `[f_out-700 ; FS/2]` **max −80,6002 dB**, avec extrémum observé vers **2 011,27 Hz**. Le rejet de la bande utile pour une décimation /10 commence à **1 966,6667 Hz**, **pas** à 1 300 Hz (cette dernière fréquence était une contrainte du deuxième étage 2 kHz).
- Contrôle supplémentaire sur une grille inclusive de **262 145 points** entre `FS/10−700` et `FS/2` : maximum d'amplitude relevé `9,33233×10^-5` vers `2 011,28 Hz`. En majorant l'interpolation du module de la réponse complexe centrée par `L×(pas/2)`, avec `L=(2π/FS)∑|n−53|·|H[n]|=0,00220611/Hz` et pas `≈0,04336 Hz`, on obtient **une borne conservatrice ≈−77,0 dB** *pour les coefficients flottants étudiés*, sur le continuum de fréquences couvert. Ce majorant mathématique tient compte de la finesse de grille, mais reste limité aux **coefficients numériques exacts utilisés dans ce calcul** ; ni erreur de quantification firmware, ni capteur physique inclus.
- Exemple de repliement d'une sinusoïde : entrée **2 000 Hz** → sortie repliée **666,667 Hz** et **−81,37 dB** après FIR. Une entrée **1 600 Hz** → repliée **1 066,667 Hz** mais avec seulement **−18,72 dB** d'atténuation ; elle demeure **hors bande métier 0–700 Hz**, et n'est pas annoncée comme correctement supprimée. **Ne pas prétendre garantir la bande 700–1 333 Hz**.
- Délai de groupe du filtre seul : `53/(80 000/3)=1,9875 ms`. Ce n'est pas la latence de publication/writer ni un modèle complet d'horodatage.

### 5.2 Causalité, chunks, témoins de reproductibilité

Oracle de référence : `signal.upfirdn(H, x, down=10)[:(len(x)+9)//10]` pour chaque axe, **état initial nul, sans émettre la queue de convolution après STOP**, phase d'émission globale `n_source mod 10 == 0`, résultat en comptes signés int16 après `np.rint` / saturation / entrelacement **X,Y,Z little-endian**. Une version `signal.lfilter` avec état `zi=zeros(106)`, index d'entrée global conservé entre chunks et émission sur `n_source%10=0` doit produire le **même nombre et la même séquence**. Après une perte FIFO, ce maintien de phase ne peut être prolongé sans créer un nouveau segment explicite.

**Test exécuté** à 160 000 échantillons source = 6 s nominales → **16 000 XYZ enregistrés**, 96 000 octets en format int16 × XYZ. Comparaison exacte, pour un signal déterministe contenant **X = 1000+12000 sin(2π·25t)**, **Y = −2000+11000 cos(2π·700t)**, **Z = 500+7000 sin(2π·1600t)**, arrondis nearest-even int16 à l'entrée : chunks répétés de taille 1, 7, 13, 31, 255, 512, 1024 et cycle variable `[1,7,10,13,31,255,512,1024]` → **16 000 sorties identiques à l'oracle batch, écart maximum 0 compte**. Ce résultat valide uniquement l'indépendance des partitions de cet oracle Python, pas la reprise de pertes ni le temps réel ARM.

| Vecteur 6 s, phase globale n=0, version float64 SciPy ci-dessus | SHA256 des 96 000 octets sortie `<i2` entrelacés |
|---|---|
| DC constant `[16384,−8192,4096]` | `7a90f77cc609e017be51c46ae4d9cac3d8476bdf2abe396742da5c504341a828` |
| Impulsion `X[0]=16384`, Y=Z=0 | `27f14a2cb1ff2d01bc9df168a1cabb756f1419b1ad78903a72aa416b28e14995` |
| Multisinusoïdes X25 / Y700 / Z1600 Hz et DC, quantifiées à l'entrée | `56696f8d06264bcb24152ec8b705aa209056309b194c1853c9c48dbb5e4e093d` |

Ces témoins servent à reproduire **exactement le même oracle et les mêmes options de format** ; des variantes `float32`/CMSIS/Q15 peuvent différer de quelques comptes et exigent des tolérances métier indépendantes, pas un faux échec SHA.

### 5.3 Quantification et pleine échelle : points de vigilance

En recalculant la réponse des **107 coefficients** quantifiés, pour une quantification indépendante de chaque tap et le même intervalle continu de fréquence :

| Représentation des coefficients FIR (simulation de quantification) | Plus forte erreur de gain en 0–700 Hz | Pic d'atténuation stopband ≥1 966,667 Hz observé | Borne de grille conservative approximative |
|---|---:|---:|---:|
| `float64` référence | ~0,000980 dB | **−80,60 dB** | **−77,01 dB** |
| `float32` coefficients | ~0,000980 dB | −80,60 dB | −77,01 dB |
| `Q15` arrondi coefficients `round(H·32768)/32768` | ~0,00144 dB | **−71,35 dB** | **−69,94 dB** |
| `Q31` arrondi coefficients | ~0,000980 dB | −80,60 dB | −77,01 dB |

**Attention** : ce tableau **ne simule pas** l'accumulateur, les arrondis intermédiaires, les décalages, les saturations d'état ou les erreurs temporelles du STM32. En Q15, `sum(|coeffs_Q15|)=47 473` unités Q15 et un calcul de somme des produits int16 pondérés a une borne absolue conservatrice de `47 473×32 768=1 555 595 264`, sous `INT32_MAX` pour le **FIR direct avec ces coefficients**, mais cela **ne qualifie pas automatiquement** la variante accélérée CMSIS-DSP ou son gain/décalage. Toute transformation de kernel exige une preuve de marge.

**Risque confirmé en simulation** : pour une inversion brusque `+32760 → −32760` de l'entrée après préconditionnement à +32760, le FIR flottant présente du ringing et les valeurs de sortie ont atteint **−35 674 à +36 382 comptes** : **six échantillons** dépassent les bornes int16 dans cet essai. Ne jamais caster directement le float calculé vers int16 en laissant déborder : **saturation explicite et indicateur de qualité** pour la sortie ; définir si des dépassements causés par le filtre sur données capteur valides doivent déclencher un avertissement distinct de la saturation physique du capteur. Ce défaut de confiance doit être traité avant le format final.

### 5.4 Portée et conclusion

- **Option de conception R1 privilégiée** : sortie ≈2 666,7 Hz par un seul FIR A107 et décimation /10, sur **les trois axes avec DC conservé**, sans deuxième filtre obligatoire.
- **Pas une qualification de production** : acquisition IIS3DWB, fréquence native réelle, stabilité d'horloge, full scale ±8g, exactitude filtre LPF capteur, bruit/alias physique pré-ADC, performance ARM/cycles et SPI2/microSD **non testés par cette analyse**.
- Le transport intégral LIVE Modbus RTU a été **examiné par principe**, puis **écarté du périmètre V1 à la demande expresse de l'utilisateur**. Ne pas rouvrir cette alternative sans nouveau besoin métier : préserver l'archive microSD et Modbus RTU pour les commandes/états/indicateurs.
- **Avant développement firmware** : reporter ces vecteurs et pièges dans le protocole R1-DSP, définir la sémantique des indicateurs de dépassement transitoire, qualifier un codec de fichier compatible PC et le temps réel du STM32. Garder le point de retour Git `a3f5ac33a728a5afd028192654c17e5b04ee8d7b` ; TR2-002 seul banc SPI2 futur, TR2-001 préservé ; contrat Codex AC1 bloqué inchangé.
