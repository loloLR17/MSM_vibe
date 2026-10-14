# Preuves P12/H3h — PoC SDMMC1 fidèle

`reference-provenance.json` : commit tiers, état propre et empreintes sources. `artifacts.json` / `artifacts-release.json` : images Debug/Release ; flags de build, logs, flashverify/readback et scripts conservés. Pas de BIN tiers redistribué : fichiers locaux reproductibles depuis source publique.

`community-init-observation.log` et `release-community-init-observation.log` : HAL_Init/WideBus HAL_OK et WIDBUS4. `release-four-bit-card-and-read.log` : read8192/HAL0 et dump avant erreur GPIOA de debugger, `release-first-read-verification.json` : oracle entier. Debug CRC, récupération et erreurs de méthode conservées.

`original-callback-integrity-128KiB.log` :16writes,2reads exacts puisCRC. `integrity-at-first-failure.json` : aucune intégrité entière prétendue à ce stade. `usb-integral-after-reset.json` et dumphex : read complet de ces128KiB viaUSB après reset, hash attendu/read égal.

`usb-phone-data-capability.json` : téléphone reconnu via le même câble/port, numéro de série omis. `usb-enumeration-settled.json` : véritableNucleo0483:572B/serial123456/card32GB ; les probes initiales négatives sont conservées sans diagnostic abusif du câble. Win32rawdisk accès refusé, partition-scoped read autorisé ensuite. Aucun privilège modifié.

`native-write-result.json` : FileStream host error. `native-aligned-result.json` : refus du garde avant écriture (original stdout vide). `native-after-guard-failure.json` : NoMedia/size0. `native-aligned-recovered-result.json` et `...verification.json` : unique essai aligné réellement exécuté après récupération,128KiB WR/RD/flush exacts. CodeNativeAligned.cs et invocation prouvent no-buffering/write-through et alignement VirtualAlloc. Ce sont des adaptations du lanceur hôte, pas du SD ou firmware original.

`final-original-release-state.log` / `final-native-enumeration.json` : dernière observation SDerror2005/R1B00 et NoMedia revenu. Aucun état stable extrapolé du succès128KiB. Logs bruts peuvent contenir CRLF/octets OEM ; manifest couvre bytes exacts, hex permet lecture indépendante. Bruts locaux : /home/lolo/dev/msm/reports/TR2-SDMMC1-POC-20261014. Pas de nouvelles campagnes à partir de ces scripts sans contrat/identité réévalués.
