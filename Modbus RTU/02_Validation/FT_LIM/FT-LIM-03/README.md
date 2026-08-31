# FT-LIM-03 — Validation, CRC et application de la configuration préparée

## Objet

FT-LIM-03 vérifie la chaîne fonctionnelle V1 qui transforme une configuration préparée en configuration active :

- cohérence et calcul du CRC préparé ;
- validité/complétude de la configuration préparée ;
- transitions d'état de `config_state` ;
- application via la commande Bloc 5 code 1 ;
- protection et cohérence de l'image active 4E ;
- cohérence de `active_config_crc`.

## Doctrine

FT-LIM-03 ne revalide ni les domaines unitaires FT-LIM-01, ni les contraintes croisées FT-LIM-02, ni les permissions FT-ACC, ni les limites structurelles FT-STR.

Une configuration invalide ou au CRC incohérent ne doit jamais être activée, même partiellement.

L'état `VALIDE` appartient à la machine d'état normative. Si l'implémentation enchaîne validation puis application de manière atomique vis-à-vis de Modbus, FT-LIM-03 n'impose pas une durée minimale d'observabilité de l'état transitoire `VALIDE` ; il impose la logique fonctionnelle et l'absence de transition interdite.

`config_error_code` existe mais aucune table détaillée normative n'est figée dans la V1 : FT-LIM-03 n'invente donc aucun code.

## Périmètre actif

1. CRC-32/IEEE 802.3 de la zone préparée 4B+4C+4D ;
2. vecteur normatif de référence `0x5207CCFC` ;
3. refus fonctionnel d'une configuration au CRC faux ;
4. validation d'une configuration complète et cohérente ;
5. transitions VIDE/BROUILLON/VALIDE/ACTIF/ERREUR_VALIDATION/ERREUR_APPLICATION ;
6. protection de l'image active en cas d'échec ;
7. cohérence `active_config_crc` avec l'image active 4E.

## Hors périmètre

- règles générales de soumission/idempotence Bloc 5 : FT-LIM-04 ;
- autres commandes Bloc 5 : FT-LIM-04 ;
- domaines unitaires : FT-LIM-01 ;
- contraintes croisées : FT-LIM-02.

## Artefacts

- `source/FT-LIM-03_source.md`
- `detaille/FT-LIM-03_detaille.md`
- `detaille/FT-LIM-03_matrice_validation.csv`
- `instancie/FT-LIM-03_execution.md`
- `instancie/FT-LIM-03_instancie_index.csv`
- `instancie/FT-LIM-03_instancie_overview.md`
