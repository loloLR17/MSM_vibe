# R4-D — preuves de diagnostic et limites

Deux probes distincts : before = mesures directes/tracées et garde avant correction ;
after = uniquement nouveaux exercices de découpage et erreur logicielle injectée.
Les numéros kind0..5 et les codes de retour bruts sont conservés dans les logs GDB.
Le cas kind3 doit échouer sur deadline, kind5 doit refuser START puis confirmer STOP.
Ils ne sont pas des réussites d'acquisition R2. Aucun R2 submit ni write FRAM dans ELF.

measurements.json dérive des logs bruts et donne cycles160MHz/us, résultats et
surcoût d'instrumentation. build-artifacts.json conserve SHA/configurations exacts.
Les fichiers .zlib sont sans perte : zlib.decompress, empreintes brutes dans JSON.
Les images NON-PRODUCTION ne sont pas des firmwares d'exploitation. FRAM préservée
contient l'authentique STARTED epoch2/run2, aucune terminalisation inventée.
Doubles backups Flash2Mio et FRAM, lectures après probes/restauration récupérables
localement sous /home/lolo/dev/msm/reports/TR2-r4d/hardware/ ; empreintes publiées.
SHA256SUMS.json porte sur tous les fichiers complets sauf lui-même. Les logs de
configuration initiale et sanitizer global échoués sont préservés, pas supprimés.
Aucun WCET électrique, FIFO/FRAM sous charge, provider temps capteur ou R6 qualifié.
