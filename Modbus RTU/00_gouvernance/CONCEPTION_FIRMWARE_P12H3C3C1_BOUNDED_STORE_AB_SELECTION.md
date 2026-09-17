# P12-H3c3-C1 — Géométrie et sélection A/B du store borné

## 1. Portée

Cette tranche définit uniquement la géométrie physique du futur journal borné et l'algorithme de lecture/sélection d'un slot logique.

Elle ne remplace pas encore `CommandJournalStore`, ne modifie pas `reserve()`, n'introduit aucune éviction et ne change aucun comportement B5.

## 2. Géométrie

- 256 slots logiques.
- 2 copies persistantes A/B par slot logique.
- 70 octets par copie, format `CommandJournalBoundedRecord` V3.
- Taille totale : 256 * 2 * 70 = 35 840 octets.
- `transaction_id` n'intervient jamais dans le calcul d'adresse physique.

Pour un `logical_slot` dans [0,255] et une `copy_index` dans [0,1] :

```text
offset = (logical_slot * 2 + copy_index) * 70
```

## 3. Classification d'une copie

Après lecture du média, une copie est :
- EMPTY si les 70 octets valent tous 0x00 ou tous 0xFF ;
- VALID si le codec V3 retourne TR2_OK ;
- UNSUPPORTED si le codec retourne TR2_ERROR_UNSUPPORTED ;
- CORRUPTED pour toute autre erreur de décodage ;
- UNAVAILABLE si la lecture du média échoue.

## 4. Sélection A/B d'un slot logique

Les deux copies sont évaluées indépendamment.

- aucune copie non vide : slot logique EMPTY ;
- une seule copie VALID : elle est courante ;
- deux copies VALID : la génération strictement la plus grande est courante ;
- deux copies VALID avec la même génération :
  - si leur contenu logique est identique, l'une peut être retenue ;
  - si leur contenu diffère, slot CORRUPTED ;
- une copie VALID + une copie CORRUPTED/UNSUPPORTED : la copie VALID est retenue ;
- aucune VALID + au moins une UNSUPPORTED : UNSUPPORTED ;
- aucune VALID + au moins une CORRUPTED : CORRUPTED.

Une erreur de lecture physique n'est jamais masquée par l'autre copie : résultat UNAVAILABLE.

## 5. Invariant power-loss

Lors d'une future mutation, la nouvelle version sera écrite dans la copie opposée avec `generation + 1`.

Lors d'une future éviction/réadmission dans le même slot logique, la génération continue également à croître. Elle n'est jamais réinitialisée à 1 tant qu'une ancienne copie du slot peut subsister.

Ainsi, après perte d'alimentation pendant l'écriture :
- l'ancienne copie valide reste sélectionnable si la nouvelle est invalide/incomplète ;
- la nouvelle copie n'est sélectionnée que si elle est intégralement valide et de génération supérieure.

## 6. Compteurs

`generation == UINT32_MAX` interdit toute mutation supplémentaire du slot. Aucun wrap silencieux.

Cette tranche ne définit pas encore les règles de `admission_order` globales ni leur reconstruction ; elles appartiennent aux tranches suivantes.

## 7. Implémentation prévue

C1 sera implémentée comme helper interne du futur store borné, avec tests unitaires dédiés sur média RAM :
- offsets extrêmes ;
- A vide/B vide ;
- A valide/B vide et inverse ;
- deux générations valides ;
- copie récente corrompue -> repli sur ancienne valide ;
- version unsupported sans copie valide ;
- égalité de génération identique/différente ;
- erreur de lecture média.

Aucun raccordement à `CommandJournal` dans C1.
