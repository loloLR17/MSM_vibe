# Résultat FT-STR-03 — GEL-MAP-V1

## Référence

- mapping : GEL-MAP-V1 ;
- commit de gel du mapping : `ff948e5917becceed7637d9c7864ec9b279be0ca` ;
- règle normative : `uint32 = registre N MSW, registre N+1 LSW`.

## Méthode

La couverture est obtenue directement en filtrant `tr2_mapping_unifie_logique.csv` sur `declared_type == uint32` puis en appliquant les invariants de `TT-STR-03-GEN-001`.

La reconstruction déterministe et l'absence d'heuristique sont définies par `GEN-002` et `GEN-003`.

## Contrôles attendus du validateur

Pour chaque `uint32` :

- exactement 2 registres ;
- adresses consécutives ;
- offsets consécutifs ;
- pour les entrées `uint32_from_split_words`, exactement deux composants sources ordonnés `_msw` puis `_lsw`.

Le validateur échoue explicitement si un invariant n'est pas satisfait et affiche le nombre de champs `uint32` contrôlés. Le nombre n'est volontairement pas recopié ici afin d'éviter une constante documentaire susceptible de diverger du mapping gelé.

## Conclusion documentaire

La méthode FT-STR-03 est reconstruite contre V1/GEL-MAP-V1. La conformité mécanique du mapping actif doit être établie par l'exécution de :

```text
python "Modbus RTU/03_Automatisation/validate_ft_str_03_multireg.py"
```

L'atomicité temporelle MSW/LSW n'est pas revendiquée par ce résultat : elle relève de FT-STR-07.
