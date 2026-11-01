# Montage breadboard TR2-002 — N8F8MHz qualifié sur gate2s, préparation8h prête

**VALIDÉ PHYSIQUE BORNÉ**, pas d'endurance ou dequalification métrologique. [Runbook actuel](PRET_8H.md), [résultat lecteur indépendant](GATE_HOST_VERIFICATION.json), [critères de temps/numériques](breadboard8-gate-collected/analysis.json), [READY](READY.json). Le capteur est celui emprunté001 selon utilisateur, pas le moduleinitial002.

## Pourquoi le passage8MHz est justifié

Le nouveau montage a rétabli la configuration/calibration à4MHz, mais son gate monteFIFO399/work983,775µs etlecollecteurinchangé refuseflags81≠0. Ce statut estwatermark+niveau, pas FULL/OVR ; l'échec global reste dans lecheckpointa7805d6d, relu113fichiers/108artefacts avantflash. Son fichierN8E0003 relu31392BauSHA exact est conservé, pasretouché.16WHO8MHz se sont ensuite révéléscorrects ; le firmwareN8F réutilise la fréquence8MHz de référenceR3, la voie duplex déjà présente et toutes les autres exigences. Aucunseuil ou vérificateur modifié pour faire passer l'essai.

Build ARM ciblé N8F réussi, text53008/data24/BSS549096 ; warningsnosys conservés. Source7b815b103a49b7fb1a8ec681a65206312b0ec16b, BIN53032B/SHA c2dd380de58f3581337b22f2957ce2265f0eb7f98c06cbeaab2d17bf64494c61 ; ELF/MAP/flags exacts inclus. Pasdemodificationfonctionnelledecode ; anciens testsPASS nonrejoués. Seulspréfixe/provenanceetdiviseurprofil diffèrent du candidat4MHz, gain/FS/ODR/FIR/v3/limites inchangés. Le champ no_flash_yet deartifact.json décrit l'instant de préparation ; FLASH_VERIFIED.json décrit leflash ensuite réellement effectué.

## Flash, calibration et unique gate

Après UID002/SN attendus, imageEentièrepréservéeetétatsûr/ODRoff/FIFObypass/TSoff vérifiés : **un flashstandard à08000000/vérification/resetnormal**, readback53032B exact/WAIT2/8MHz. Aucun masserase/OptionBytes/reset supplémentaire, capteurinitialisationnonforcée.

Init/calibrationunefois : WHO7B, bonCID/CSD/capacité, FAT32 etnomsneufs/libre contrôlés enlecture ; calibration24µs/198brackets/6577ancres sanserreur. Un gatecmd3N8F0003 :52592XYZ/5260sorties/31560Butiles/68records,2000143µsMCUnominales. QPC indépendant intérieur1,0390489s, aucune duréeSI2s prétendue. Zéroflags/FULL/OVR/rail/clip/SD/fileoverflow/tracefaute, contrôleurinchangé pass_references=true/103sorties préfixe exactes. Work641,10625/1000µs, service1348,51875/2500µs, gap1606/5000µs ; FIFO48/file5120B. STOP133µs/drain8421µs/pileobservée1104B/plancher2992B ;8XYZ finaux sansancrefuture nonémis, paspertemasquée enCOMPLETE. CTRL1_XL=AC relu, configuration8g contrôlée parfirmware, pascalibrationd'amplitude.

## Relecture indépendante complète et portée

FIFOarrêté/filevide : f_openREAD/f_read16×8192B/f_close, copie131072B complète viaRAM/GDB, compteurSDwritesinchangé pendantcopie. Puis lecteurPythonordinaire **inchangé** read_campaign.verify : COMPLETE,68records/5260XYZ, tousCRC/index/session/footer/qualité/SHA exacts, digestproducteurRAM30e0de23…939a3. SHAentierfichiere8227c33…54e6d ; originalcomplet N8F0003.TR2 inclus en zlib sans perte. Les16chunks régénérables restent locaux et sont représentés par le fichiercomplet archivé. Ce chemin est FatFs→GDB→fichierhôte, **pas un retrait au lecteur natifPC** ; pasdepreuve de tenueàcoupure/rétention ni de l'ensemble duFAT/anciensfichiers, lesquelsn'ontpasétére-hachés. Écritures limitées auxfichiersneufs/allocationsqualifiées, aucune suppression/format/réparation.

## Préparation et limites finales

READY=true après `launch_8h.py --check-only` réellementPASS : UID/image53032B/CIDCMD10/FAT32/libre≥20480clusters/N8F0005absent, COM5parent002 etouvertureexclusive, T1000secteur etveille/hibernationAC0. AucunSTARTlong ni fichierN8F0005 créé. Lanceurqualifié adaptéchemins/préfixe uniquement ; lockpersistant/seulsignalLANCE8H conservés. Préflight àrépéterbrièvementausignal, aucunbuild/retestavantSTARTsiétatpréservé. Toutecoupure ou nouveauchangementmodule/montage invalide ceprêt ; moduleinitial002 et endurance restentNONVÉRIFIÉS.

Débogueur détaché aprèspréflight, serveurs terminés, FIFOoff/writeroff/queue0/clockfault0/CShaute etstage4/run3/error0 confirmés. N8F8MHz conservé surcarte, sensorborrowed001/profil8g, aucunrestaurationdeEjustifiée. AucuneopérationFRAM/COM4/001/serviceWindows/protection effectuée danscettequalification. Unincidentscriptpréparationcopié référençait uncheminhôte inexistant ; échec avantIO corrigé verscopie locale duPSreadonly, PREPARATION_CORRECTION.json ; pas unpremiergatephysiquecaché. Les fichiersWindowsCRLF sont archivészlib sansmodifierunseuloctet.

Conclusion : le banc002 et lecapteurfonctionnent dansce montage pourcegate. LaNucleo002 n'estpas démontréedéfectueuse ; le rôleexclusif de la breadboard n'estpasprouvé car recâblage/cyclealimentation/état initial ontchangé. AucunGOproduction/V1/AC1/metrologie/8h. Mission8h IN_PROGRESS enattenteLANCE8H, pasDONE ; objectif dequalificationcourte du nouveaumontage atteint danscetteportée.
