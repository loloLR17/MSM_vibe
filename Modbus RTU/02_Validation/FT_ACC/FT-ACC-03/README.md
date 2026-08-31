# FT-ACC-03 — Refus d’écriture sur les champs RO

## Objet
Valider que tout champ déclaré **RO non réservé** par le mapping V1 refuse toute opération d’écriture Modbus.

## Hiérarchie documentaire
1. spécification V1 gelée ;
2. mapping unifié gelé ;
3. `source/` ;
4. `detaille/` ;
5. `instancie/`.

Le mapping est la source opérationnelle d’instanciation ; il ne constitue pas une norme indépendante.

## Doctrine applicable
Conformément à **GEL-GOV-02**, une écriture visant un champ RO est un accès Modbus invalide. Elle doit :
- produire une exception Modbus standard appropriée ;
- ne produire aucune modification imputable à la requête rejetée ;
- ne provoquer aucune exécution partielle ;
- présenter un comportement déterministe.

Pour une cible naturellement dynamique, l’absence d’effet se juge causalement : une évolution autonome normale reste admissible et n’est pas assimilée à une modification provoquée par l’écriture rejetée.

## Périmètre
FT-ACC-03 couvre exclusivement les champs logiques **RO non réservés**.

Sont hors périmètre :
- zones réservées : FT-ACC-04 ;
- écritures composites invalides : FT-ACC-06 ;
- adresses inexistantes en lecture et structure d’exposition : FT-STR-06 ;
- validité métier des valeurs : FT-LIM ;
- cohérence structurelle : FT-STR.

## Structure active
- `source/` : doctrine et exigence de validation ;
- `detaille/` : 2 génériques, mono-registre et multi-registres ;
- `instancie/` : index exhaustif de 129 cibles + overview ;
- `archive_pre_renforcement/` : ancien référentiel complet, y compris les anciennes fiches individuelles.

Les anciennes mini-fiches instanciées ne sont plus exécutables dans le chemin actif : les critères d’acceptation sont portés par les génériques, tandis que l’index fixe l’identité et l’adresse de chaque cible.

## Couverture V1
Couverture active : **129 champs logiques RO non réservés**, répartis sur les blocs 0 à 7.

`B3_RESERVED_0` est exclu de FT-ACC-03 et appartient à FT-ACC-04.
