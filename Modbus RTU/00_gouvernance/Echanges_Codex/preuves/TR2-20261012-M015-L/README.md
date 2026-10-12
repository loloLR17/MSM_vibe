# M015-L — preuves de la tentative WHO unique et de la restauration

Préflight confirmé utilisateur, seconde identité différente du TR2, backup Flash2MiB double identique. Artefacts K réutilisés, aucune compilation/test rejoué. Une image WHO seule flashée/verify/readback, un trigger, un payload reçu00 au lieu7B. Stage6/result2/cleanup0, aucun write capteur. Pas de polling, FIFO strict ou STATUS10.

Logs GDB/CLI complets normalisés UTF8/LF (originaux cp1252/CRLF et binaires conservés localement), manifest des hashes publié/original. Inventaire initial transcrit depuis sortie réelle ; ce JSON n'est pas une copie du stdout binaire. Identité confirmée ensuite par logs CLI/GDB et UID brut hashé.

Première capture s'est arrêtée au symbole macro GPIOC absent de GDB, avant dumps. Capture complémentaire uniquement mémoire, sans trigger/reset/flash/requête capteur, adresse NS42020800 vérifiée dans CMSIS. Payload00 et trace40octets conservés, RX pendant commande non capturé ; commande8F reconstructible depuis source, pas preuve wire. Globals controls/XYZ/FIFO zéroBSS nonmesurés, pas de CSVXYZ inventé.

Avantrestauration, Flash entière lue et bytes après08008000 comparés exacts au backup. Seuls secteurs0..3 restaurés depuis backupfrais32KiB ; Flash2MiB entière ensuite byte-identique/SHAfcea004e… ; reset GDB aprèsdernierCLI, bootobservé/faults0 puis detach/serveursexit0. Firmware initial nonidentifié/qualifiéfonctionnellement ; il n'est pas assimilé à M014-H. Aucun accès au TR2 principal, aucun média/COM7/RS485/OptionBytes/HOTPLUG/masserase.

Originaux : /home/lolo/dev/msm/reports/TR2-mission015-L-20261012T195811Z. Le projet testé reste e222dc8ddb7cfd0829c655533ac8e646220721f2. Cause de WHO00 non établie ; lectureGPIOIDR/ODR200 établit étatMCU final CShaut/MISOdigitalbas, pas mesure électrique ni historique du fil.
