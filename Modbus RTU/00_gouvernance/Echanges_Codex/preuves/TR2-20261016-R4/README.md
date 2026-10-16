# R4 — preuves finales et limites de qualification

Rapport actif : ../../DERNIER_RAPPORT.md. Code évalué f3ae3a6.
Mission techniquement bloquée pour la campagne R2 complète : STARTED indéterminé
préservé, sans reçu causal autorisant sa clôture. Aucun effacement ni redispatch.

- slot-split-full-validation.log :136/136tests et ARM PASS après dernière correction.
- slot-split-sanitize-test.log :4suites ASan/UBSan PASS ; model-final-extracted.log
  reprend les métriques de LastTest.log de cette validation, sans relancer le modèle.
- hardware/bench1(bench-*),bench2,bench3 : erreurs timeout/attente premier-mot,
  préfixe FIFO readonly puis contrôle final interrompu par GDB, R1 persisté puis
  refus d'ordre d'initialisation. Aucun RUN complet FIFO/FRAM déclaré PASS.
- hardware/bench4-r2-result.log :1030us>1000us, arrêt sûr et START indéterminé.
  bench4-readonly-mount-attribution-ex.log : reproduction readonly/pile dans
  validate_control. Erreur attendue de GDB à l'arrêt dans un appel conservée ;
  les commandes -ex suivantes capturent réellement pile/work et détachent.
- hardware/bench5-recovery-result.log : correction sans relèvement de limite,
  terminal10/error0,907usmax,0write ; statut9 conservé. Comparaison complète FRAM
  avant/après dans le script de vérification : aucune modification.
- hardware/restoration-summary.json,restore-*.log : restauration standard14pages
  depuis backup puis lecture2Mio identique ; FRAM inchangée, historique/tail protégés,
  M016 commissioning WAIT et GDB détaché. Pas nouveau firmware LIVE qualifié.
- resources/*.su : cadres statiques compilateur O0 ARM, pas pile high-water physique.
  module-inventory.json/loc-code.json donnent méthode et périmètre de comptage.

Les sorties brutes, y compris échecs d'outillage et CRLF Windows, sont conservées.
Une commande globale retournant0 ne remplace pas chaque résultat. Backups/binaries
restent locaux dans /home/lolo/dev/msm/reports/TR2-r4/hardware ; leurs SHA publiés
ne prétendent pas remplacer le contenu. Aucun trafic HP/COM7 ni RS485 électrique.
SHA256SUMS.json vérifie chaque preuve publiée sauf lui-même ; scripts matériels
archivés comme preuves, à réutiliser uniquement après préflight/permissions actuels.
