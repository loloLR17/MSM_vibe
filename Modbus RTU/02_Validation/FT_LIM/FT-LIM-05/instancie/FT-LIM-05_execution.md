# FT-LIM-05 — Procédure d’exécution

1. Lire 6001 `total_campaign_count` et consigner N.
2. Ne jamais modifier 6001 pour fabriquer un N : le registre est RO.
3. Pour chaque test applicable, écrire uniquement 6003 via une fonction Modbus autorisée par le mapping.
4. Une valeur hors domaine fonctionnel ne doit pas être transformée en test d’accès invalide : l’écriture doit être acceptée au niveau Modbus.
5. Lire 6004 `selected_campaign_valid` comme oracle principal.
6. Si 6004=1, lire l’entrée sélectionnée et vérifier au minimum `campaign_id!=0` ainsi que la cohérence sémantique attendue.
7. Si 6004=0, ne pas interpréter 6012..6057 comme métadonnées valides et ne pas exiger une valeur numérique particulière de ces registres.
8. Pour les changements de sélection, terminer une réponse Modbus avant d’effectuer l’écriture suivante sur 6003.

## Gestion des préconditions

- N=0 : exécuter les cas inventaire vide ; les cas exigeant N>0 sont N/A.
- N=1 : première et dernière campagne sont le même index 0 ; ne pas prétendre à deux valeurs distinctes.
- N=2 : pas d’index intérieur strict ; G04 N/A.
- N>2 : G04 applicable.
- N=65535 : G10 applicable ; 65535 est la première valeur hors plage.

## Verdict

PASS seulement si les oracles fonctionnels applicables sont satisfaits. Toute précondition impossible à obtenir naturellement est notée N/A avec motif documenté.
