# Preuves du seul essai M016 LOAD-ONLY

Code évalué : c6c56b63e02cb78f176f278195dc114a6b7cbe76. Un seul RUN, arrêté sur overflow, sans retry.

- `single-load-run.log/.gdb` : capture au point terminal, sans halt intermédiaire ; `unique-load-attempt.json` identifie le RUN.
- `run-analysis.json` : métriques brutes, vingt latences et calculs hors ligne ; `write-latencies.csv` et `write-latencies.bin.hex` conservent la série et le dump brut.
- `integral-written-prefix-readonly.log/.gdb` : vingt lectures réussies du seul préfixe écrit, après arrêt du producteur, sans reset ni nouvelle écriture.
- `integral-prefix-verification.json` : comparaison de chacun des 1 310 720 octets, première divergence absente, SHA-256 réel et attendu par chunk. Le compteur embarqué `load_verified` reste à zéro : l'erreur charge empêche sa vérification automatique. La preuve d'intégrité provient de la continuation en lecture seule et de l'oracle Python indépendant.
- Scripts `hardware.py`, `m016_load_{transition,init,run,verify_prefix}.py` : commandes réellement utilisées. Les chemins sont ceux du T1000 ; ne pas les exécuter comme une autorisation de répétition.
- Logs de builds, identité, transition sûre, init et readback Flash conservés ; `artifacts.json` identifie BIN/ELF/MAP. `load-image.bin.hex` représente le BIN évalué.
- `manifest.json` : SHA-256 de chaque fichier de ce dossier, sauf lui-même.

Bruts `prefix-read-000.bin` à `019.bin` conservés localement dans `/home/lolo/dev/msm/reports/TR2-M016-1BIT-LOAD-20261013T215507Z`, avec le dump Flash et les logs. Les 1,3 MB de lectures ne sont pas ajoutés au dépôt public ; leurs empreintes sont publiées. Les hashes seuls ne remplacent pas la comparaison octet par octet effectivement exécutée.

Les quantiles utilisent le rang supérieur `ceil(p*n)` sur seulement vingt écritures. Le débit utile est octets écrits / durée totale de charge, attente initiale du premier chunk incluse. Il ne représente pas un benchmark isolé de la carte.
