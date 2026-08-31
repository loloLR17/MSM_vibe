# Audit mapping unifié ↔ Spécification Modbus RTU V1

## Statut

**AUDIT EN COURS — MAPPING NON GELÉ**

Référence normative :

- `01_Specification_source/bloc0.md` à `bloc7.md` ;
- `01_Specification_source/charte_typage.md`.

Hiérarchie applicable :

```text
Spécification V1 gelée
        ↓
mapping_unifie dérivé
        ↓
tests
```

Toute divergence est corrigée dans le mapping dérivé. La V1 n’est pas modifiée pour faire correspondre la spécification à un artefact ancien.

## Résultat structurel intermédiaire

La couverture d’adresses annoncée est continue pour les huit blocs, mais ce contrôle historique ne détecte pas les doublons/chevauchements ni les divergences de type.

Blocs sans divergence structurelle identifiée à cette étape :

- Bloc 1 ;
- Bloc 2 ;
- Bloc 5.

Ces constats restent subordonnés au contrôle croisé final après correction globale des CSV.

## Registre des anomalies certaines

| ID | Bloc | Élément | Mapping actuel | V1 gelée | Action |
|---|---:|---|---|---|---|
| MAP-01 | 0 | adresse 20 | `reserved` | `reserved_0` | corriger nom dans brut et logique |
| MAP-02 | 3 | `B3_RESERVED_0` | type `réservé` | `uint16[8]` | corriger type |
| MAP-03 | 4 | adresse 4017 | deux entrées : `reserved` et `reserved_4B_17` | une seule entrée `reserved_4B_0` | supprimer doublon et corriger nom |
| MAP-04 | 4 | `axes_enable_mask` | `uint16` | `bitfield16` | corriger type |
| MAP-05 | 4 | `full_scale_code` | `uint16` | `enum16` | corriger type |
| MAP-06 | 4 | `acquisition_mode` | `uint16` | `enum16` | corriger type |
| MAP-07 | 4 | `storage_mode` | `uint16` | `enum16` | corriger type |
| MAP-08 | 4 | `active_axes_enable_mask` | `uint16` | `bitfield16` | corriger type |
| MAP-09 | 4 | `active_full_scale_code` | `uint16` | `enum16` | corriger type |
| MAP-10 | 4 | `active_acquisition_mode` | `uint16` | `enum16` | corriger type |
| MAP-11 | 4 | `active_storage_mode` | `uint16` | `enum16` | corriger type |
| MAP-12 | 6 | `reserved_6A` | `uint16\\[2]` | `uint16[2]` | normaliser échappement |
| MAP-13 | 6 | `reserved_6B` | `uint16\\[6]` | `uint16[6]` | normaliser échappement |
| MAP-14 | 7 | `reset_cause` | `uint16` | `enum16` | corriger type |
| MAP-15 | 7 | `reserved_7A` | `uint16\\[2]` | `uint16[2]` | normaliser échappement |

## Cause probable

Les anomalies observées indiquent que le problème n’est pas seulement situé dans la vue logique : plusieurs divergences existent déjà dans `tr2_mapping_unifie_brut.csv`.

Le mécanisme historique de déduplication retire les doublons exacts, mais peut conserver deux lignes documentaires différentes décrivant la même adresse, comme observé en Bloc 4 à l’adresse 4017.

Le contrôle de couverture historique ne suffit donc pas à garantir la conformité du mapping.

## Contrôles obligatoires avant gel

Le mapping V1 ne pourra être gelé qu’après vérification automatique ou systématique de :

1. couverture complète des plages normatives ;
2. absence de trou ;
3. absence de doublon/chevauchement non justifié ;
4. unicité de représentation de chaque adresse physique dans la vue brute ;
5. conformité des noms ;
6. conformité des types ;
7. conformité des accès ;
8. conformité des tailles / nombres de registres ;
9. cohérence des regroupements logiques `uint32` et ASCII ;
10. conformité des champs réservés à la convention `reserved_*` ;
11. absence de types hors charte ;
12. audit croisé final V1 ↔ mapping corrigé.

## Critère de gel proposé

Le statut **GEL-MAP-V1** ne pourra être attribué que si :

- toutes les anomalies MAP-XX sont closes ;
- le mapping brut est conforme à la V1 ;
- le mapping logique est une transformation déterministe et sans perte du mapping brut ;
- le contrôle de couverture ne masque aucun chevauchement ;
- une passe finale indépendante ne détecte aucune divergence normative.
