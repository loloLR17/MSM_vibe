# FT-STR-05 — Registres réservés et sentinelles structurelles

## Objet

FT-STR-05 valide les règles structurelles applicables aux zones explicitement réservées et l'absence de sentinelles implicites.

## Référentiel

Ordre de vérité :
1. spécification Modbus RTU V1 (`bloc0.md` à `bloc7.md`, `charte_typage.md`) ;
2. GEL-MAP-V1, mapping dérivé ;
3. `source/FT-STR-05.md` ;
4. `detaille/TT-STR-05-GEN-XXX.md` ;
5. `instancie/`.

Aucune règle héritée ne peut modifier V1.

## Architecture

- `source/` : définition de la sous-famille ;
- `detaille/` : contrôles génériques indépendants des adresses ;
- `instancie/` : application compacte à GEL-MAP-V1 ;
- `archive_pre_renforcement/` : historique uniquement.

## Doctrine

- un registre n'est réservé que si V1 le définit comme tel ;
- les registres réservés doivent lire `0` lorsque V1 l'impose ;
- `0` n'est jamais une sentinelle globale ;
- une signification telle que « absent », « invalide » ou « non renseigné » n'existe que si V1 la définit explicitement pour le champ concerné ;
- la stabilité temporelle relève de FT-STR-07 ;
- le refus des écritures sur réservés relève de FT-ACC et GEL-GOV-02.

## Validation active

La couverture active est dérivée directement de GEL-MAP-V1. Le validateur `03_Automatisation/validate_ft_str_05_reserved.py` contrôle mécaniquement l'identification, la géométrie et la couverture des zones réservées. La valeur réellement lue à `0` reste un contrôle d'implémentation à exécuter sur la cible.

Les anciennes fiches instanciées sont archivées et ne participent plus à la validation active.
