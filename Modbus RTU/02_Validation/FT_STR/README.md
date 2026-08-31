# FT-STR — Conformité structurelle Modbus RTU

## Objet

Ce répertoire contient la famille de validation **FT-STR**, dédiée à la conformité structurelle de l'interface Modbus RTU du capteur TR2.

FT-STR vérifie que l'image registre exposée est conforme à la spécification V1, correctement typée et encodée, lisible selon les règles Modbus retenues, et cohérente sans hypothèse implicite côté centrale.

## Hiérarchie documentaire

L'ordre de vérité est :

1. spécification Modbus RTU V1 gelée : `01_Specification_source/bloc0.md` à `bloc7.md` et `charte_typage.md` ;
2. `mapping_unifie` / GEL-MAP-V1, artefact dérivé de la V1 ;
3. fiches `source/` des sous-familles FT-STR ;
4. tests génériques `detaille/` ;
5. tests et résultats `instancie/`.

En cas de contradiction, le niveau supérieur prévaut. Aucune évolution de V1 ne doit être introduite silencieusement depuis un artefact de validation.

## Pipeline gelé FT-STR

L'ordre de validation de référence est :

1. **FT-STR-08 — Conformité documentaire** ;
2. **FT-STR-01 — Structure des plages** ;
3. **FT-STR-02 — Typage des champs** ;
4. **FT-STR-03 — Encodage multi-registres** ;
5. **FT-STR-04 — ASCII fixe** ;
6. **FT-STR-05 — Registres réservés et sentinelles structurelles** ;
7. **FT-STR-06 — Accessibilité et découpage de lecture Modbus** ;
8. **FT-STR-07 — Stabilité d'image et cohérence temporelle**.

La numérotation FT-STR-01 à FT-STR-08 est gelée pour la V1.

## Doctrine active

- une lecture portant sur une sous-plage d'adresses valides n'est pas invalide du seul fait qu'elle est partielle ;
- le découpage des champs logiques (`uint32`, ASCII fixe, etc.) ne crée pas de frontière Modbus implicite ;
- les intervalles non exposés entre blocs ne sont pas des registres réservés ;
- `0` n'est jamais une sentinelle globale : une signification spéciale n'existe que si V1 la définit explicitement pour le champ concerné ;
- les réservés sont identifiés sans ambiguïté selon les noms de la V1 et du mapping dérivé ;
- les écritures interdites et refus d'accès relèvent de FT-ACC et de GEL-GOV-02 ;
- les données dynamiques peuvent varier entre requêtes ; FT-STR-07 contrôle la stabilité des données réellement statiques et la cohérence d'un même instant logique à l'intérieur d'une réponse multi-registres ;
- aucun résultat terrain n'est déclaré PASS sans exécution ou preuve adaptée.

## Arborescence

Chaque sous-famille suit, lorsque pertinent :

```text
FT-STR-0X/
├── README.md
├── source/
├── detaille/
├── instancie/
└── archive_pre_renforcement/
```

`archive_pre_renforcement/` est historique uniquement et ne participe jamais à la validation active.

## Références

- `Specifications.md` : cadrage global de la famille FT-STR ;
- `00_gouvernance/CHARTE_ARBORESCENCE.md` : structure et pipeline ;
- `00_gouvernance/CHARTE_ACCES_INVALIDES.md` : doctrine des accès invalides ;
- `02_Validation/mapping_unifie/` : mapping GEL-MAP-V1 ;
- `03_Automatisation/` : validateurs mécaniques associés.

## Statut

Famille reconstruite et auditée sous GEL-GOV / GEL-MAP-V1. Les résultats qui nécessitent une cible réelle restent explicitement à exécuter ; le gel documentaire ne doit pas être interprété comme un PASS matériel du firmware.
