# TT-ACC-02-B5-002 — Bloc 5 — cmd_request_transaction_id

## Objectif
Vérifier que le champ logique `cmd_request_transaction_id` déclaré `RW` accepte une écriture conforme et restitue la valeur écrite en lecture immédiate.

## Exigence(s) couverte(s)
- FT-ACC-02
- Mapping unifié logique TR2
- Attribut d’accès : `RW`

## Référence mapping
- Adresse début : 5001
- Adresse fin : 5001
- Type déclaré : uint16
- Nombre de registres : 1
- Description : Identifiant de transaction de la commande

## Préconditions
- FT-STR validée
- FT-ACC-01 validée
- Accès Modbus opérationnel
- Simulateur en état nominal stable
- Valeur initiale lisible avant écriture

## Données d’entrée
- Adresse de départ : 5001
- Longueur : 1
- Valeur de test : mot de test différent de la valeur initiale

## Contrôle de frontière
- Champ RW suivant attendu : cmd_request_param1
- Adresse de début du champ RW suivant attendue : 5002
- Vérifier l’absence de débordement de l’écriture au-delà de la plage `5001..5001`.

## Scénario / étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `5001` pour obtenir la valeur initiale.
2. Écrire exactement `1` registre(s) sur la même plage avec une valeur de test différente de la valeur initiale.
3. Contrôler l’absence d’exception Modbus sur l’écriture.
4. Relire exactement `1` registre(s) à partir de l'adresse `5001`.
5. Vérifier que la valeur relue correspond à la valeur écrite.
6. Vérifier que l’opération reste bornée à la plage `5001..5001`.

## Résultat attendu
- l’écriture est acceptée ;
- aucune exception Modbus n’est renvoyée ;
- la relecture contient exactement `1` registre(s) ;
- la valeur relue correspond à la valeur écrite ;
- le champ `cmd_request_transaction_id` est modifiable conformément au mapping.

## Critères d’acceptation
- écriture réussie ;
- relecture cohérente ;
- longueur conforme ;
- comportement cohérent avec l’attribut d’accès `RW` ;
- aucune divergence mapping ↔ comportement réel.

## Mode d’exécution
- simulateur déterministe
- automatisable

## Automatisation possible
Oui

## Traces à conserver
- trame de lecture initiale ;
- trame d’écriture ;
- trame de lecture de contrôle ;
- valeur initiale ;
- valeur écrite ;
- valeur relue ;
- verdict ;
- anomalie associée le cas échéant.

## Niveau de criticité
P0

## Remarques / limites
- cette fiche valide l’accessibilité en écriture et la cohérence write → read ;
- la validité métier de la valeur écrite est hors périmètre FT-ACC-02.
