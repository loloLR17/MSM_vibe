# FT-LIM-06 — Procédure d’exécution

1. Lire `total_campaign_count` N.
2. Lire `storage_health_status` et vérifier son domaine 0..3.
3. Si N=0, les tests par campagne sont N/A ; ne pas fabriquer de campagne.
4. Si N>0, sélectionner uniquement des indices valides selon FT-LIM-05.
5. Vérifier `selected_campaign_valid=1` avant d’interpréter l’entrée.
6. Lire les métadonnées de la campagne sélectionnée.
7. Vérifier domaines `campaign_state`, `data_integrity_status` et `campaign_id!=0`.
8. Si `campaign_state=2`, vérifier impérativement `end_timestamp=0`.
9. Consigner `start_timestamp`, `end_timestamp` et `duration_s`. En l’absence d’une formule/tolérance normative plus précise, ce point reste `TRACE_ONLY` et ne peut produire seul un PASS/FAIL.
10. Répéter sur les campagnes disponibles si le coût d’exécution le permet ; l’index d’instances décrit les classes de preuve, pas une limite au nombre de campagnes inspectées.

## Règle RO
Aucun champ testé ici n’est modifié directement. Les états absents de l’environnement sont N/A/non observés.
