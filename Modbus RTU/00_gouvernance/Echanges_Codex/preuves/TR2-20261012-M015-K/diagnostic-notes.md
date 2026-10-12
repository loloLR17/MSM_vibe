# Incidents de préparation, sans accès matériel

- Fetch gitlink ST d995549f0e5a17e57c81b0eafc367fbdc0043e1b : GitHub «not our ref». Driver officiel figé séparément f83e74f, manifeste explicite.
- Téléchargement des PDF ST par urllib et curl Windows : timeouts, pas de PDF binaire récupéré. Lecture web officielle consultée ; aucune empreinte PDF inventée. Outil PDF préparé dans cache local, inutilisé ; aucune configuration système ou Codex modifiée.
- Premier mock ne compilait pas : nom OUTX_L_XL absent ; corrigé avec le symbole réel OUTX_L_A de l'en-tête ST. Pas de modification de test attendu.
- Première exécution FIFO mock refusait la configuration malgré09=0A : inspection GDB hôte établit getter batching NOT_BATCHED (switch0x10). Garde brut09=0A explicite ajouté, getter conservé comme télémétrie, sources vendeur inchangées.
- Test suivant passe le refus de niveau strict03F, les erreurs, le cas nominal STATUS10 et les bornes. Version finale ajoute WHO readonly et horloge MSI16MHz/SPI2MHz explicite ; validations relancées uniquement pour ces changements, aucune validation I/J/TR2 rejouée.
