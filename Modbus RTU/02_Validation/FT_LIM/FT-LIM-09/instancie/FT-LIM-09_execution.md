# FT-LIM-09 — Procédure d’exécution

1. Lire les domaines RO du Bloc 2 et contrôler les enums et bits réservés.
2. Relever un état temporel de référence.
3. Écrire une valeur `prepared_time` adaptée à la campagne, sans soumettre la commande 2.
4. Vérifier que `current_time` poursuit son évolution normale et n’est pas remplacé immédiatement par la valeur préparée.
5. Observer `last_sync_time` avant toute synchronisation : il doit rester inchangé.
6. Lorsque les préconditions FT-LIM-04 de la commande 2 sont satisfaites, soumettre une transaction nouvelle et observer l’application temporelle.
7. Vérifier que la synchronisation effective correspond à l’événement attendu sans imposer une précision non définie.
8. Vérifier la monotonie de `current_time` hors synchronisation et accepter une correction lors de la synchronisation.
9. Vérifier la mise à jour de `last_sync_time` après synchronisation effective.
10. Observer `time_since_sync` sur plusieurs lectures et contrôler sa cohérence temporelle sans formule/tolérance inventée.
11. Consigner les grandeurs sans domaine métier en TRACE_ONLY.

Les résultats d’acceptation/refus de commande restent attribués à FT-LIM-04.
