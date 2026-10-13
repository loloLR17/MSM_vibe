# M015-U — Matrice de transposition et qualification bornée

Référence initiale c27621f ; TR2-001 SN003500463235510B37333439 / UID004A003D3842501820333157. Utilisateur confirme reconnexion. Seconde NUCLEO interdite d’accès.

| Élément | Banc T | Principal U | Risque et contrôle |
|---|---|---|---|
|CPU/PLL/VOS/Flash|160MHz/MSI4×80÷2/VOS1/4WS|Initialisation principale conservée|RCC/PWR/Flash et SystemCoreClock relus avant trigger|
|SPI3|HSI16/div4/mode3/PC9-12/threshold1|Profil OFF par défaut, mêmes réglages explicites|HSI indépendant ; clock API puis registres ; pas simple diviseur PCLK|
|SPI1 FRAM|Absent|2,5MHz principal inchangé ; lecture seule|Backup complet double lecture et comparaison après capture ; write absent ELF|
|SDMMC2|Absent|Non initialisé dans le profil|HAL_SD_Read/Write et bringup absents ELF ; partition non accédée|
|UART/Modbus|Absent|Profil exclusif sans transport|Pas de coexistence simultanée revendiquée ; retour exact M014-H et B0/B1 après|
|Driver ST|Vendor immuable|Sources mêmes chemins, compilées|Deux getters ST historiques défectueux ; niveau10bits/parser local, aucun patch silencieux|
|Initialisation capteur|Fonction officielle générée et fingerprintée|Même générateur/fonction, même port|Erreur ignorée par exemple convertie en refus par callback/longjmp|
|Transport|HAL RX O2, split TX/RX sous CS|Adaptateur des callbacks T attaché au hspi3 principal|Snapshots avant HALCloseTransfer, reste/offset/SR/CS/error, reserve cleanup|
|FIFO RAM|4096XYZ+512TS,128mots/burst|Parser T réutilisé ; admission READY distincte B1|Timeout500ms/polls400/tags/parité/mod4/TS/saturation/OVR/FULL ; pas preuve perte zéro|
|RAM/pile|Journaux bornés|BSS572472 + heap512/stack2048, RAM786432|Plus de200KiB de marge statique ; pas mesure de high-water pile|
|Services M014|Image permanente qualifiée|Production inchangée ; image historique restaurée|Build production actuel différent, pas assimilé à l’image historique ; pas flash de ce build|

Le profil TR2_STM32_ST_FIFO_WINDOW est exclusif des propriétaires médias/transport/acquisition. ARM n’atteint aucune routine du harness après l’entrée noreturn. Le transfert est borné, monolithique, déclenché une fois en RAM et inaccessible via V1. Les données ne sont READY qu’après résultat, cleanup, compte4096/512/4608, journal intègre, WHO et mise au repos. Ce choix minimal protège M014 ; une coexistence simultanée et l’activation V1 exigent un service coopératif ultérieur, pas M016 ici.

Validation : suite complète123 tests et ARM défaut, profil ARM nouveau, ARM production, quatre refus de ressources concurrents, symboles médias/UART interdits absents. Faux périphérique parser normal/partiel/vide/timeout/flags/parité/compteurs/config/saturation et positions IO ; predicates SPI CS/offset/OVR testés ; admission finale refuse cleanup/quantités/journal/flags/WHO. Gardes d’identité/clock/hardware SPI READY vérifiées sur cible ; pas prétention de mock matériel exhaustif.

Plan physique : backup Flash double exact M014-H ; dump domaine persistant RAM avant flash. Flash normal image vérifiée ; readback entier et tail ; reset GDB après CLI ; double FRAM readonly avant trigger et comparaison hash historique ; préflight clock/WHO préparatoire ; un trigger officiel WHO/config puis4096 ; dumps bruts, reconstruction indépendante. Sur premier défaut STOP, cleanup puis restoration pages impactées depuis backup frais, verify Flash2MiB et boot ; deux lectures B0/B1 bornées si HP accessible. Pas autre acquisition ni8MHz, ni essai secondaire.
