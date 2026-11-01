# Préparation 8h INVALIDÉE — état actuel BLOCKED au 01/11/2026 local

**Ne pas exécuter le lanceur historique ci-dessous.** La session N8A préparée a été perdue après redémarrage observé. Les reprises ont échoué et le firmware actuellement laissé est N8E duplex4MHz, code4307b95c/BIN59e65cd6…bc186, stage99/error1/run1 après refus FIFO_CTRL4=06 au lieu86. FIFO arrêté, writer inactif, SD au repos ; READY=false. Aucun long n'a démarré. Voir [état et preuves actuels](duplex4-new-wires-20261101/README.md) et DERNIER_RAPPORT.md.

La remise en préparation exige d'abord la résolution du défaut SPI/configuration puis une calibration et un gate FIFO/SD conformes. L'identité WHO7B seule ne suffit pas. Les instructions ci-dessous sont **l'instantané historique de la préparation initiale réussie**, conservé pour traçabilité ; leurs états, image et commande ne décrivent plus le banc actuel.

---

# PRÊT 8H — préparation acquise, attendre LANCE 8H

Mission `TR2-20261031-R3-8H-FASTSTART-PC-MACHINE`, préparation du31/10/2026. **Aucun long n'a démarré.** Image source `d5af0828e370780320bda67fac5d537bf3fd5016`, BIN54556B/SHA`c31904feca48fd06d6102eee40b54b373bbfe5e9896d0ddc007434cf253ddc17`, ELF/SHA`45831eccc8744bce99c5f2c51b5d79564ee9c6416c6986d7a47f6e733376fbcf`. Build isolé `arm/`, v3/int16, FIR107Q15/décimation10/244µg nominal/±8g logique inchangés ; garde30600000000µsMCU, préallocation640MiB, prefixN8A. RelectureFlash entière identique. Un seul flash normal avec reset de programmation, aucun reset supplémentaire.

Cible réellement vérifiée : TR2-002, SN0032001F3235510C37333439, UID001B0046/38425018/20333157 ; WHO7B. Carte FAT32/CID0353445344333247858bf0923c01927b/62333952secteurs/32768Bcluster. Libre initial927219clusters ; gate4clusters, long20480 requis. N8A0003/5 tous deux absents avant gate ; maintenant **N8A0003 créé et vérifié**, **N8A0005 absent**. FA_CREATE_NEW/f_expand/sync/close/continuité existants restent fail-closed ; pas de format/suppression ni écrasement de fichier historique.

État conservé : **attente idle, stage4/run3/error0**, FIFOoff/writeroff/file0/CShaute, debugger détaché, serveur fermé. Le WAIT initial stage2 a été vérifié après flash ; le gate terminé laisse stage4, prêt pour commande5. **Conserver cette session alimentée et ne pas faire RESET** : un redémarrage perd l'état préparé et le lanceur le refuse, sans reflasher/rejouer automatiquement. Ne pas retirer/changer la microSD. Le T1000 est sur secteur, veille et hibernation automatiques AC0 observées, COM5 lié au SN002 et ouverture exclusive testée ; ces faits périssables seront recontrôlés au départ. Aucun accèsTR2-001/COM4/FRAM/production.

## Un seul message utilisateur

**`LANCE 8H`** vaut confirmation du capteur fixé au plancher du PC machine, configuration identique et alimentation/ST-LINK maintenus. Avant ce message, aucune exécution avec `--signal`.

Codex exécute seulement :

```bash
python3 /home/lolo/dev/msm/reports/TR2-r3-8h-faststart-20261031/launch_8h.py --signal 'LANCE 8H'
```

Le lanceur vérifie brièvement identité/Flash/idle, CID réellement relu CMD10, FAT32/libre/nomlong absent, secteur/veille/COM5exclusif, puis réutilise `start_detached.py n8a-long 3 5 28801`. Verrou local atomique `LONG_START_ATTEMPT.json` avant la première tentative : même en échec, aucune relance automatique ni suppression du verrou. Préfixe et chemin natif du témoin uniques ; firmware commande5 seulement depuis run3, FA_CREATE_NEW et vérification intégrale du gate acquis avant allocation. Pas de build, flash, nouveau gate, tests, Git, analyse R5 ou transfert de données avant START.

Observation initiale ACTIVE seulement après premier groupeSD acquitté ; conserver `n8a-long-launched.json`, `START_LATENCY.json` et `LONG_START_ATTEMPT.json`. La durée de lancement réelle sera mesurée, aucune promesse de latence inventée. Dès ACTIVE confirmé, GDB détaché, serveur/lanceur terminés ; seul témoinWindows natif passif sur COM5. Aucun processusCodex/WSL/GDB de surveillance8h.

## Contrôles acquis et preuves

- Sources fonctionnelles identiques àc3cd40ce, tests salvage et cross-build canonique déjàPASS **non rejoués** ; comparaisonGit réelle dans validation-reuse.json. Build ARM cibléN8A seul exécuté, succès.
- Nouveau profil/nom/image justifient **un seul gate2s** :2,000357sMCU/52936natifs/5294sorties/31764Butiles. QPC intérieur1,0422059s, session6755700148813124 ; zéroFIFO/SD/queue/rails/clips. Work684.281/service1262.013/gap1521.000µs sous1000/2500/5000, FIFO46/file5120B, STOP152µs/drain9277µs.
- Préfixe numérique103sorties indépendant exact ; fichiergate entièrement relu FatFs, CRC/index/session/compte/footer/SHA producteurRAM conformes. Ce n'est pas une extractionPC de la carte et ne qualifie pas8h.
- `launch_8h.py --check-only` exécuté réellement : préflight completPASS, aucun START. Gardes d'entrée horslignePASS : signal absent/erroné refusé, étatnonpréparé refusé, tentativeéchouée verrouillée et secondeinvocation refusée avant matériel. Pas de démonstration fictive d'un STARTlong.

## Fin et récupération

MCU assure seul STOP/flush autonome à30600sMCU nominales, marge historique conservée ; témoinQPC demande28801s intérieures puis attend terminal. CheminWindows : `%TEMP%\TR2-R3-N8A-20261031-n8a-long\` (ready/queries/summary). Une fermeture de fenêtre ou huit heures à l'horloge ne prouvent pas SDquiescente. `summary.json` sans failure avec terminalflags1284 peut indiquer la fin observée ; flags1282/error exigent diagnostic, pas COMPLETE inventé.

**Pas de voie existante qualifiée de sauvegarde automatique du digest RAM sansCodex identifiée** : le témoin transporte session/compte/état, pas le SHA. Revenir versCodex après fin pour relever le digest et les diagnostics avant coupure si possible. Si déplacement indispensable après fin/quiescence confirmée, la coupure perd ce digest indépendant mais le fichierFAT32 reste potentiellement exploitable ; conserver cette réserve comme pour6h. Ne jamais retirer la carte en activité. Aucune interventionphysique demandée maintenant.

Reprise même mission : lirejournauxQPC natifs, confirmer terminal, adapter collect_after_terminal.py pour run5 si succès (sans nouveauSTART), sauvegarderRAMavantcoupure, demander ensuite transfertcarteauPC, copie conservatoire puis vérification intégrale strictev3/CRC/temps/comptes/SHA et clips. Si clips/error26, préserver préfixe et qualitédégradée avec inspect_quality_failure.py, sans fauxCOMPLETE. Pas de tag ni qualification8h avant fichiersPC et preuves ; aucune donnée massive à envoyer avantl'appareillage.
