---
mission_id: TR2-20261013-M015-T-RESUME-PUBLICATION
status: BLOCKED
base_ref: main
base_sha: 7d762176b9fdc14d3c0cdce32bd6b3ce9d135d35
initial_head: 7f7fc2a4e41b9734fce65123a371fcef8f154639
result_sha: 790301635b56bc23170a8d05c590bb42ec4ba8f4
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-T reprise — 4096 XYZ qualifiés à CPU160/SPI4 et SPI8

## Résultat et portée

Les preuves locales T retrouvées sont auditées indépendamment : **4096 XYZ +512 timestamps à4MHz**,4082triplets distincts. Après publication de cet acquis, l'unique essai8MHz conditionnel réussit également : **4096 XYZ +512timestamps**,4086triplets distincts. Chaque capture comporte4608mots FIFO réels, reconstruits byte par byte depuis les transactions SPI et comparés au buffer RAM officiel.

Tags2/4, parité, TAG_CNT XYZ modulo4, saturation, erreurs HAL/SPI, restants/offsets/CS avant fermeture, flags FIFO FULL/OVR/latch et journal sont contrôlés :aucun défaut observé sur les fenêtres. Entre timestamps :8XYZ et pas24ticks dans les deux captures. **VALIDÉ PHYSIQUE BORNÉ4096**, sans garantie globale de perte zéro, d'endurance, d'excitation calibrée ou d'industrialisation. La FRAM, le stockage, FFT et la chaîne vibratoire V1 ne sont pas qualifiés par ce résultat.

| Configuration | Preuve | Résultat |
|---|---|---|
|S CPU16/SPI4, sourceMSI/PCLK3|Historique, pas rejoué|OVR+timeout,63/896octets consommés,0XYZadmis|
|T CPU160/SPI4, sourceHSI16/div4|Capture historique unique retrouvée et auditée|4096XYZ+512TS ;niveauFIFOmax187/final91 ;cleanup0|
|Reprise CPU160/SPI8, sourceHSI16/div2|Unique nouvelle acquisition|4096XYZ+512TS ;niveauFIFOmax186/final32 ;cleanup0|

Le passage S→T change CPU/APB, VOS, latenceFlash et sourceSPI MSI→HSI. Il soutient l'amélioration du service RX avec plus de ressources mais ne démontre pas une causalité exclusivement CPU. Les fréquences SCK sont calculées, sans oscilloscope ni calibration des oscillateurs. Entre T4 et reprise8, CPU160/HSI16, capteur, ODR, mode3, parser, buffers, journaux et guards restent identiques ; seul le diviseur SPI change.

## Marges mesurées et correction de l'analyse antérieure

Les bruts DWT restent inchangés ; conversion160cycles/µs. Pour les segments128mots/896octets, le budget nominal de production est4,266667ms (210000octets/s avecTS/8).

| Mesure |CPU160/SPI4|CPU160/SPI8|
|---|---:|---:|
|Phase payload complète moyenne|1,807226ms|0,908141ms|
|Service actif moyen jusqu'à la fin du premier statut de boucle suivant|2,048578ms|1,118683ms|
|Maximum de cette enveloppe active|2,049513ms|1,120031ms|
|Budget moins cette enveloppe maximale|2,217154ms|3,146635ms|
|Cadence entre départs, incluant attentes de remplissage|4,285707ms|4,257142ms|
|Borne haute moyenne hors attentes minimales demandées|3,142850ms|2,628571ms|
|Maximum de cette borne / marge restante nominale|4,000225 /0,266442ms|3,000200 /1,266467ms|
|Durée fenêtre incluant démarrage/attentes/contrôles|168ms|166ms|

L'enveloppe active couvre burst, parsing, copie et contrôles jusqu'au premier statut suivant, avant toute attenteHAL_Delay. La cadence incluant les attentes suit l'arrivée des données : elle n'est pas le débit maximal du service. Soustraire seulement1ms par attente demandée donne une **borne haute** du temps occupé, car HAL_Delay(1) et sa quantification peuvent durer davantage ; ce n'est pas une mesure isolée du CPU.

L'ancien calcul local non publié comptait aussi les statuts après burst comme des attentes. Ils n'en déclenchent aucune : correction75→40 demandes pourT4, appuyée sur les sources et indices exacts. Les fichiers importés timing-comparison.json et service-intervals.csv conservent l'ancienne analyse, identifiée comme erronée sur ce point. Les fichiers final-comparison.json, independent-audit.py et *-service.csv donnent les valeurs corrigées. Aucun test physique ni build4 n'a été répété.

L'essai8 était justifié après l'audit4 par un gain théorique de0,896ms/segment,≈21% du budget. Le gain actif observé est **0,929895ms**,≈22% de ce budget. Cela offre une réserve de service supplémentaire pour une future intégration ; aucune capacité FFT/stockage effective n'est démontrée.4MHz est la plus faible fréquence qualifiée4096 sur ce banc ;8MHz est une option bornée avec plus de marge, à requalifier dans le système final.

## Provenance, horloges et contrôles

T4 :code `7d762176b9fdc14d3c0cdce32bd6b3ce9d135d35`, BIN SHA256 `0091c1e84bf7a351ec3f5d7ca4f00109ce72924633ff11a57ad0027750f0b2f3`, ELF `9a9d568b1032e650ede0de5c7558dfdbf0bd4c0522f9cd3817d49deb547d6b57`. Flash relue byte-identique. Raw4608mots SHA `04e071f2f851ddb48293da0d329b7bf96f995d689c87ff913409daf7be1a33c6`.

T8 :code `790301635b56bc23170a8d05c590bb42ec4ba8f4`, BIN SHA256 `7ea208296dd539d8c02cde6d9664131212bf2229eeccbd0c2620d5910392f312`. ELF etmap dansartifacts.json ;readback Flash exact. Raw4608mots SHA `c0e1059656a7f41607a2e530c9f4bd2cd2f7b5af77b9956d1e1a5864dac21d68`.

Les deux captures contrôlent SystemCoreClock/bench_core_hz160MHz, RCC PLL/HSIready, CCIPR3=10h(SPI3HSI16), PCLK3div1, VOSR7C000(VOS1ready/boost), FLASH_ACR4WS. CFG1 MBR1(div4) pourT4 et MBR0(div2) pourT8. [STM32U575 constructeur](https://www.st.com/resource/en/datasheet/stm32u575zi.pdf) et CubeU5v1.9.0 définissent ces sources/limites ; [IIS3DWB](https://www.st.com/resource/en/datasheet/iis3dwb.pdf) limiteSCK10MHz. Rails/câblage proviennent des confirmations utilisateur antérieures ; fronts et marge électrique non mesurés. La réussite bornée ne remplace pas une qualification électrique industrielle.

Le préflight T initial s'était arrêté GDBexit7 pour une mauvaise adresseSPI3 dans le script, avant le trigger. Source CMSIS vérifiée :SPI3_BASE_NS46002000,CFG1offset8 ;préflight corrigé sans flash/reset supplémentaire. Refus, script et logs originaux préservés. Cette erreur documentaire n'altère pas les données T acquises après correction.

Dans la reprise, seul CMake autorise maintenant le coupleCPU160/div2 explicitement prévu par le contrat. Sourceplateforme, driverST, parser, instrumentation, contrôles etcleanup ne changent pas. Une compilation ARM8 réussit ; les compilations/tests T/S et références historiques sont conservés, sans répétition. Pas de DMA/refactoring. Une erreur de préparation de copie (SameFileError sur chemin absolu) a interrompu la première copie sans modifier les originaux ; correction du nom de destination avant sauvegarde.

## Matériel final et préservation

Seconde NUCLEO seulement :SN `0032001F3235510C37333439`,UID `001B00463842501820333157`. Pour8 :identité avant flash, empreintes, SWDnormal08000000/verify/readback, resetGDB après dernierCLI, préflightRCC/PWR/Flash et un seul trigger. GDB/serveurs8exit0.329traces/33168octets journal/658snapshots,141statuts/38bursts ;buffers etréserves nonépuisés, loggingoverflow0.

Registres actifs relus :07..0A=00/01/0A/86,WHO7B,CTRL1AC,CTRL3=44,CTRL10=20. Cleanup0/CTRL1=0C(XLoff),FIFO_CTRL4=80(bypass),CTRL10=00(TSoff),CFSR/HFSR0,UIDconfirmé/detach. La seconde carte reste sur l'image8inactive aprèscleanup. Aucun backup/restauration2MiB,mass erase,OptionBytes/RDP,geste physique ou autre acquisition. **Aucun accès matériel au primaire/M014-H/FRAM/SD/Modbus/HP/supervision**.

OriginauxT et échecsS conservés ;contrat et checkpointT archivés byte-exacts depuis7d76217, archivesS inchangées. Worktrees historiques/HEAD et deuxstashes préservés ;dépôt principal reste82169af. Les restrictions du sandbox courant ont nécessité des approbations techniques de création/écriture/Git et d'accès au banc ;elles n'ont pas été contournées ni modifiées.

## Publication et suite

Checkpoint de reprise `790301635b56bc23170a8d05c590bb42ec4ba8f4` publié avant l'essai8 :96fichiers intégralementrelus depuisorigin/main,byte-identiques. Rapport final et preuves préparés ;BLOCKED provisoire uniquement pour la publication/relecture restante, pas pour la fenêtre4096 désormais qualifiée dans cette portée.

[Corpus, scripts et matrice](preuves/TR2-20261013-M015-T-RESUME/README.md). OriginauxT : `/home/lolo/dev/msm/reports/TR2-mission015-T-20261012T233912Z` ;reprise : `/home/lolo/dev/msm/reports/TR2-mission015-T-resume-20261013T084635Z`.

Journal etroadmap actualisés pour4/8 et la correction de marge ;M015 reste INPROGRESS pour intégrationprimaire/continu/endurance/FFT/stockage/V1. Suite : figer cette méthode d'acquisition et choisir sous contrat une intégration bornée au primaire en préservant M014-H/FRAM/SD, avec sourceSPI/diviseur explicitement contrôlés. Pas de transposition automatique des diviseurs sur une autre clocktree, pas de nouvelle campagne ici.
