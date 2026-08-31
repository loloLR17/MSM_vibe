# FT-ACC-04 — Overview instancié

Couverture active reconstruite selon GEL-GOV-02.

- 18 zones réservées logiques uniques ;
- une seule instanciation par zone ;
- doublon historique de l’adresse 4017 supprimé ;
- `B3_RESERVED_0` intégré à FT-ACC-04 ;
- aucune lecture structurelle redondante avec FT-STR ;
- toute écriture réservée doit être rejetée par exception Modbus, sans effet ni exécution partielle.
