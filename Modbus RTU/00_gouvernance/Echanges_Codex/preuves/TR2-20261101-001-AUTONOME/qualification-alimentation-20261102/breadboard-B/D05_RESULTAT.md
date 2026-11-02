# D05 — Module B déplacé sur breadboard, fils conservés

Utilisateur confirme déplacement hors tension, TR2-001 sous ST-LINK, seul le
capteur déplacé, lecteur SD et jumpers inchangés. Contrairement au plan initial,
aucun fil de TR2-002 n'est prélevé : montage002 à préserver. Module B toujours sur001.
Les précédentes mesures VDD/VDDIO3,315V sont antérieures à ce déplacement.

Source flashée inchangéeacb14279/D01,50912B entièrement relus et identiques,
SHA6a00ec329c3b16cbf1c01fc3661014ad6f1fed05732a60fecbc338d6a7bb98d2.
UID001 exact. Aucun build, flash, reset ou START par Codex. Une seule transaction
conservée : lecture0x0F/1octet, payload **00** au lieu de7B ; HALstatus/error0,
RX/TXrestants0, valid1. Cela prouve la valeur reçue, pas une conformité électrique.

Firmware arrêté99/error1/run0, avant FS et calibration (FSreadback0,198anciens
brackets non réutilisés : compteursactuels0). FIFO initialisé mais jamais démarré,
FIFO runningfalse/stoppedflagfalse ; ne pas transformer ce flagfalse en échecSTOP.
Writerinactif, commandes/lectures/écrituresSD0, initialized/write_enabledfalse,
CS SD haute, GDBdétaché. Aucune commande FRAM/COM4 ni accès002. Aucun fichier créé.

Résultat différent deD04(error7/calibration avecWHO7B) : après déplacementbreadboard,
identification refusée. Cela ne prouve pas une destruction du module ou du MCU.
Alimentation/masse/contacts et implantation de la breadboard doivent d'abord être
vérifiés, en conservant les preuves et sans relancer de calibration/60s à l'aveugle.

Prochaine action demandée à l'utilisateur : DC alimenté/arrêté, noirGNDNucleoCN8-11,
VDDmodule/VDDIOmodule/GNDmodule ; attentes3,3/3,3/0V. Pas de continuité sous tension,
ni déplacement de fil alimenté. Mesures nouvelles encore en attente ; aucune
modification de réglage, fréquence, garde ou firmware.
