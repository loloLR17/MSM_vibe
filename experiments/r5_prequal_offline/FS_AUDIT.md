# Revue de pleine échelle — faits séparés, 31 octobre 2026

## Sources et registre

- Spécification fabricant gratuite : [ST DS12569 Rev8, juillet2025](https://www.st.com/resource/en/datasheet/iis3dwb.pdf), tableau3 et note6 pages5–6, registre/table30 page32. Sensibilités 61/122/244/488µg par compte pour ±2/4/8/16g. Bruit typique trois axes : XY75 et Z110µg/√Hz ; pas garanti, caractérisé **100Hz–6,3kHz**, indépendant du FS. Le gain numérique n'est pas un gain de bruit physique à1Hz.
- [ST AN5444 Rev3](https://www.st.com/resource/en/application_note/an5444-iis3dwb-ultrawide-bandwidth-lownoise-3axis-digital-vibration-sensor-stmicroelectronics.pdf), chaîne de filtrage, mise en route/settling et FIFO. Ce document ne remplace pas une caractérisation instrumentale de bruit en bande basse.
- Source recorder réellement archivée : `c3cd40ce9e96cef2241fb5bc6e8c83acd2a3824e`. `experiments/r3_durable_fat32/src/main.c` écrit logiquement8g/244µg au header et appelle `fifo_live_start(...,7,2,...)`. `experiments/iis3dwb_st_reference/src/fifo_live.c` mappe code2 vers enum ST8g, configure puis lit CTRL1_XL au START ; attendu `0xAC`. Ces deux fichiers et le FIR sont inchangés depuis le SHA source archivé (diff Git vérifié).
- Preuve observée historique : `TR2-r3-6h-data-20261031/six-short-data/result.json` contient ctrl1=172 au court réussi. Le long a dû passer la garde au START selon le code, mais la lecture du registre du long n'a pas été récupérée après coupure. Il n'existe pas de preuve continue du registre pendant6h : **FS_PHYSIQUE_NON_VÉRIFIÉ sur toute la durée**. La métadonnée seule n'est pas une mesure du registre. L'ancien AC1/TR2-001 bloqué ne décrit pas ce banc d'archivage TR2-002.
- État physique actuel : **NON RELU**, aucun STM32/COM/USB/debug/flash autorisé ici. Aucune prétention sur la plage d'une nouvelle traversée.

## Six heures archivées : les extrêmes changent la décision

MESURÉ sur codes archivés, conversion nominale sous hypothèse244µg/count :
X [−0,931836 ; +1,258064]g ; Y [−1,258552 ; +0,700036]g ;
Z [−4,631120 ; +7,676728]g, hors11 warmups FIR. Moyenne Z1,018618g.
Le quantile Z99,9% vaut1,160464g, **pas une borne**. Un épisode de41 sorties
Z au-delà de2g se trouve entre17613,441565 et17614,769104s MCU ; cinq dépassent4g,
deux6g, une7g. Pic index46876121 à17613,446449s. Sa cause est inconnue :
ni choc mécanique, ni défaut électrique/SPI, ni mouvement du support ne sont prouvés.

| Plage nominale proposée | Marge X à la plus forte valeur absolue | Marge Y | Marge Z |
|---|---:|---:|---:|
| ±2g | +0,741936g | +0,741448g | −5,676728g |
| ±4g | +2,741936g | +2,741448g | −3,676728g |
| ±8g | +6,741936g | +6,741448g | +0,323272g |

ESTIMÉ : ces marges concernent la sortie FIR/décimée ; elles ne bornent pas le
signal natif. On ne suppose pas que le FIR est sans dépassement : sa norme L1
vaut1,4486083984375. Avec l'arrondi ±0,122mg, le pic exige au moins5,299297g
nominaux parmi les entrées numériques contributrices **si la chaîne documentée
est correcte**. Ainsi, même un possible dépassement FIR ne justifie pas une
réduction aveugle à±4g. Cela ne démontre pas l'accélération analogique réelle.

SOURCE_RAIL=0 / OUTPUT_CLIP=0 dans l'archive acceptée. Dans le code réel,
SOURCE_RAIL signale un code exactement −32768/32767 présent dans l'historique
FIR107 contribuant à une sortie ; les historiques se recouvrent tous les10codes.
OUTPUT_CLIP signale le clamp int16 après convolution/arrondi. Le premier ne
qualifie pas l'absence de saturation analogique, de presque-rail ou de crêtes
entre instants ; la queue native sans sortie au STOP reste non archivée. Le
second ne décrit pas directement la saturation capteur. Ne pas déduire un
zéro risque de saturation des flags ou des seuls RMS de bande.

## Budget et recommandation

| CALCUL NOMINAL | ±8g | ±4g | ±2g |
|---|---:|---:|---:|
| Pas mg/count | 0,244 | 0,122 | 0,061 |
| Borne ±0,5LSB mg | 0,122 | 0,061 | 0,0305 |
| RMS bruit uniforme de quantification mg (hypothèse) | 0,07044 | 0,03522 | 0,01761 |
| Densité quantification entrée µg/√Hz à26667Hz (hypothèse blanche) | 0,610 | 0,305 | 0,1525 |
| Densité arrondi sortie µg/√Hz à2666,7Hz (hypothèse blanche distincte) | 1,929 | 0,964 | 0,482 |

Les densités supposent des erreurs uniformes décorrélées ; ce n'est pas garanti
pour un petit sinus cohérent. Bruit analogique, arrondi de sortie et repliement
HF restent distincts. Le FIR archive filtre fortement la haute fréquence, sans
créer une preuve de bruit à1Hz. La simulation connue avant quantification
explore0,3/3/30mg avec et sans bruit blanc hypothétique : à0,3mg sans bruit,
biais RMS plat +2,35% en8g contre−0,066% en2g ; à3mg, environ0,011%/0,008%.
Ce gain dépend du signal ; avec le bruit hypothétique il n'est pas monotone.
Ni ces modèles ni une requantification de l'archive ne recréent les bits perdus.
Détails et vitesse/spectre dans resolution-simulation.json.

**Recommandation : conserver±8g pour le montage/campagne actuels.** ±2g peut
être étudié sur un point de confort maîtrisé ; ±4g n'est pas automatiquement
le compromis sûr au vu de cet épisode. Aucun changement aujourd'hui. Déterminer
d'abord la crédibilité des transitoires et mesurer le bruit basse fréquence.

## Courte qualification future, à autoriser séparément

Mission STM32 dédiée avec flash/debug explicites : cible et image identifiées,
readback FS au START et contrôles périodiques/fin, même pose rigide et axes repérés,
horloge et chaîne FIR identiques, gravité conservée et compteurs rails/clips.
Préférer deux capteurs synchrones ; sinon alternance8g/4g/2g explicitement
non simultanée avec état de machine/conditions consignés. Commencer au repos
sur8g, enregistrer PSD de bruit1–100Hz et température, puis signal de référence
amplitude/fréquence connues ; paliers bornés et conservation des bruts natifs
autour de tout événement. Arrêter le palier réduit à proximité de la saturation
sans changer les seuils après coup. Comparer SNR/PSD/RMS et marges réelles,
pas seulement le nombre de comptes. Ni nouvelle capture ni câblage/flash préparé
pour exécution automatique par ce document.
