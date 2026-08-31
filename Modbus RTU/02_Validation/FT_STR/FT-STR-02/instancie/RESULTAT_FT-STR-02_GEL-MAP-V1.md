# Résultat FT-STR-02 — GEL-MAP-V1

## Références

- sous-famille : `FT-STR-02 — Typage des champs`
- mapping : `GEL-MAP-V1`
- gel mapping : `ff948e5917becceed7637d9c7864ec9b279be0ca`
- charte normative : `01_Specification_source/charte_typage.md`
- validateur mécanique : `03_Automatisation/validate_ft_str_02_typing.py`

## Couverture attendue

| Bloc | Champs logiques |
|---:|---:|
| 0 | 10 |
| 1 | 18 |
| 2 | 12 |
| 3 | 26 |
| 4 | 66 |
| 5 | 18 |
| 6 | 20 |
| 7 | 13 |
| **Total** | **183** |

## Contrôles

Les contrôles instanciés de FT-STR-02 portent directement sur les 183 lignes de `tr2_mapping_unifie_logique.csv` :

- type déclaré présent ;
- type protocolaire autorisé, ou notation documentaire `uint16[n]` ;
- taille en registres compatible avec le type ;
- absence des types historiques/interdits actifs (`enum`, `bitfield`, `float*`, `réservé` utilisé comme type) ;
- couverture exacte des huit blocs.

Les règles suivantes ne sont volontairement pas contrôlées ici : ordre MSW/LSW, contenu ASCII, valeurs métier, accessibilité Modbus, droits d'accès et stabilité temporelle.

## Statut

**CONFORME sur le référentiel documentaire gelé GEL-MAP-V1**, sous réserve que l'exécution du validateur mécanique retourne un code de sortie `0` sur la copie de travail considérée.

Le mapping GEL-MAP-V1 a déjà été audité et gelé par rapport à V1. Le présent résultat ne transforme pas le mapping en source normative : V1 et `charte_typage.md` restent supérieurs conformément à GEL-GOV-01.

## Historique

Les anciennes fiches champ-par-champ FT-STR-02 ont été conservées dans `archive_pre_renforcement/instancie_legacy/`. Elles ne doivent plus être utilisées pour la validation active, car certaines représentent un état antérieur à GEL-MAP-V1.
