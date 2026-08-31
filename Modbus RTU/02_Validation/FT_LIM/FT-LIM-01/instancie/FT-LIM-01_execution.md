# FT-LIM-01 — Procédure d'exécution des instanciations

## 1. Principe

Les 68 lignes de `FT-LIM-01_instancie_index.csv` sont les instanciations exécutables de FT-LIM-01.

Elles utilisent les cas génériques `LIM01-G01` à `LIM01-G06` définis dans `../detaille/FT-LIM-01_detaille.md`.

Aucune fiche Markdown individuelle n'est nécessaire : l'index contient la cible, l'adresse, la valeur, la classe, le cas générique et le verdict attendu.

## 2. Baseline

Utiliser une configuration préparée connue valide conforme à la V1. Le vecteur normatif CRC du Bloc 4 peut servir de point de départ lorsqu'il satisfait les préconditions de l'instance.

Pour les essais modifiant une valeur de 4B+4C+4D, recalculer le CRC conformément à la V1 avant de demander l'application.

## 3. Séquence par ligne

1. restaurer la baseline ;
2. prendre un snapshot de l'image active ;
3. écrire la valeur `test_value` à l'adresse logique indiquée ;
4. lire le champ préparé pour vérifier la prise en compte de l'écriture ;
5. vérifier l'état `BROUILLON` après modification préparée ;
6. recalculer et écrire le CRC si nécessaire ;
7. soumettre la commande Bloc 5 code `1` avec un `transaction_id` neuf et un front montant de `submit` ;
8. relever `config_state`, le résultat de commande et l'image active ;
9. appliquer le verdict défini par le cas générique et la colonne `expected` ;
10. consigner PASS / FAIL / N-A avec justification.

## 4. Conditions particulières

- `VALID_IF_BASELINE_VALID` : toutes les autres valeurs et préconditions doivent rester valides.
- `VALID_IF_CROSS_CONSTRAINTS_SATISFIED` : choisir la baseline de sorte que la contrainte `indicator_period_ms >= durée de fenêtre` soit satisfaite ; la contrainte elle-même sera testée dans FT-LIM-02.
- `VALID_IF_CAPACITY_COMPATIBLE` : exécuter seulement si la capacité utilisable annoncée par le firmware est compatible.
- `CONFIG_NOT_VALIDATABLE` : la valeur est une sentinelle ou un état « non configuré » explicitement interdit pour validation.

## 5. Non-régression FT-ACC

Une écriture sur la cible RW ne doit jamais être transformée en test d'accès interdit.

Si l'écriture Modbus échoue uniquement en raison de la valeur fonctionnelle choisie, l'instance FT-LIM-01 est en échec et une anomalie de doctrine doit être ouverte.
