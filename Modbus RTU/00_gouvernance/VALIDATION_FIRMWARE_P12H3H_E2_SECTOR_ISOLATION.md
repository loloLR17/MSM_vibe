# P12-H3h-E2 — Validation isolation sectorielle du préfixe durable

## Statut

VALIDÉ ET GELÉ.

Baseline logicielle validée :

```text
a243c60cd666473e660fbaec54dc05b659fb6ebf
Firmware: fix H3h-E2 extent capacity test
```

## Objet

E2 corrige la composition D5-B / D5-C avant raccordement au backend SDMMC E1 :
une écriture future ne doit pas réécrire un secteur de 512 octets contenant
des données appartenant à un préfixe déjà rendu durable.

## Règle gelée

- granularité d'isolation physique : 512 octets ;
- tout bloc D5 commence sur une frontière de 512 octets ;
- D5-A reste compact et inchangé ;
- l'extent physique est la taille D5-A encodée arrondie au multiple supérieur
  de 512 ;
- le padding réservé n'appartient ni au payload, ni au CRC, ni au préfixe
  logique durable ;
- writer et reader avancent du même extent physique arrondi ;
- D5-C rejette un `data_base` récupéré non aligné ;
- une campagne suivante commence sur une frontière sectorielle.

Exemple gelé pour un record de 16 octets :

```text
32 header + 16 payload + 4 CRC = 52 octets encodés
extent physique réservé = 512 octets
```

## Validation locale

Validation rapportée sur la baseline ci-dessus :

```text
101/101 tests verts
cross-build STM32 : OK
```

Les tests couvrent notamment :

- calcul 52 -> 512 de l'extent physique ;
- rejet des offsets initiaux non alignés ;
- writer/reader avec progression sector-alignée ;
- refus lorsqu'un extent physique complet ne tient plus sur le média ;
- placement sector-aligné de la campagne suivante ;
- recovery du préfixe logique exact ;
- preuve qu'une queue écrite après checkpoint démarre dans un secteur distinct
  du dernier extent durable.

## Invariant obtenu

Après flush du dernier bloc d'un checkpoint puis `sync()`, le prochain offset
D5 est sur une nouvelle frontière de 512 octets. Une écriture ultérieure via
E1 ne nécessite donc pas de RMW du secteur contenant le dernier bloc du
préfixe durable précédent.

E2 ne suppose ni ne revendique l'atomicité power-loss interne d'un secteur SD.

## Limites maintenues

Ce gel ne démontre pas encore :

- la composition D5-C -> E1 sur microSD physique ;
- le comportement lors d'une coupure réelle pendant append/checkpoint ;
- le pire stall d'écriture SD ;
- le découplage acquisition/stockage ;
- une taille de buffer de production ;
- le raccordement au runtime STM32 ;
- la publication de capacité dans `ConfigurationValidationEnvironment`.

Les points d'audit D4-B encore ouverts doivent rester suivis séparément,
notamment les comparaisons de descriptors, la géométrie des copies metadata et
les contrôles de bornes/duplication avant qualification de production.
