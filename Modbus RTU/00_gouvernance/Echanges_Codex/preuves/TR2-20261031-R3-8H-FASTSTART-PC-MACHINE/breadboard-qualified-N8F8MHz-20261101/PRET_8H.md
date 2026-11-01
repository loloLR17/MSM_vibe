# PRÊT 8H — N8F duplex8MHz, montage breadboard qualifié sur gate2s

Préparation du01/11/2026UTC, **aucun long lancé**. TR2-002 SN0032001F3235510C37333439/UID001B0046/38425018/20333157, module capteur emprunté001 et filsDupont via breadboard selon utilisateur. Carte FAT32 test CID0353445344333247858bf0923c01927b,62333952secteurs,cluster32768B ; secteurT1000/veilleAC0/hibernationAC0/COM5parent002 etouvertureexclusive vérifiés.

Code `7b815b103a49b7fb1a8ec681a65206312b0ec16b`, BIN53032B/SHA `c2dd380de58f3581337b22f2957ce2265f0eb7f98c06cbeaab2d17bf64494c61`. ImageN8F entièrement relue identique, profilexistantduplexSPI3=8MHz/SPI2inchangé, ODR26,7kXYZ/s/±8g/244µg nominal/FIR107Q15/10/v3int16 inchangés ; garde30600000000µsMCU etpréallocationlong640MiB. Aucun algorithme/seuil assoupli. Un flashstandard/resetdeprogrammation, aucun reset supplémentaire ; sauvegardeN8E intacte.

Calibration24µs/198brackets/6577ancres ; gateuniqueN8F0003.TR2 COMPLETE :2000143µsMCUnominales/52592natifs/5260sorties31560B, zérofaute/flags0/rails/clips. FIFO48/file5120B, work641,10625/service1348,51875/gap1606µs sous1000/2500/5000, STOP133µs/drain8421µs,103sorties numériques préfixe exactes. Fichier131072B entièrement copié parFatFs/GDBversfichierhôte puis lecteurPython strict :68records/5260XYZ/SHAamont `30e0de232ceddbab4697a6a4768ea75b3bfcf88d012e4045c181c10dbf6939a3` exact. Pas une extractionlecteurPC ni une endurancequalifiée ; QPCintérieur1,0390489s seulement, pas2sSI étalonnées.

ÉtatPRÉPARÉ conservé : stage4/run3/error0/FIFOoff/writeroff/filevide/SDquiet/CShaute/GDBdétaché. N8F0003 créé et vérifié ; **N8F0005 absent**, CID reluCMD10/libre≥20480clusters vérifiés par check-only. GateE4MHz/N8E0003 etanciens fichiers nonremplacés ; E4restesousréserve de marge etrefusflagsbruts conservé.

## Lancement sur signal explicite uniquement

`LANCE 8H` confirme montageimmobileauplancherPCmachine et alimentation/ST-LINK maintenus selon contrat. Après ce signal seulement :

```bash
python3 /home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/launch_8h.py --signal 'LANCE 8H'
```

Réutilisation du lanceur existant adapté aux chemins/préfixeN8F seulement. LockLONG_START_ATTEMPT.json crééexclusivement avanttentative, mêmeéchouée ; aucunretryautomatique/suppression. Préflightbref/cibleimage/carte/nomlibre/COM5exclusif/secteur/veille, puiscommande5unique etobservateurQPCpassif ; aucune compilation/flash/gate/Git avantdépart. Check-only exécuté, aucunSTARTlongtesté. Avantactivité, toute incertitude exige inspection ; nepastransformer un timeout enSTOPprouvé.

Conserver l'alimentation si l'on veut conserver cet étatRAM préparé. Toutecoupure/reset/changementcapteur/câblage invalide la préparation : vérifierl'état et les préconditions avantdépart, sansforcerREADY niécraserN8F0003 ; nepasreflasher/rejouer automatiquement un gateoccupé. Les limites d'échantillonnage/declipping restentstrictes ; pasde8hvalidée avantfichierfinal.

## Fin / récupération

Attendre le terminalQPC/firmware etSDquiescente ; une fenêtrefermée ouhuitheureshorloge ne suffitpas. AucunCodex/GDB ne doit rester attachépendantlong. AprèsSTOP, relever digestRAMavantcoupure si possible ; voieexistante n'assurepasd'autosauvegardeSHAindépendante. Coupureavantrelevéperd ce témoinvolatile, sansrendreautomatiquementfichierinexploitable. RetraitSD uniquementhorsalimentationaprèsquiescence etdemandeutilisateur, puisvraiecopielecteurPC/CRC/index/footer/SHA/qualité. Encasdeclips/error26, conserverdonnéesetqualitédégradée sansCOMPLETEfictif. TR2-001/FRAM/COM4/serviceWindows horspérimètre, aucunchangement de sécurité.
