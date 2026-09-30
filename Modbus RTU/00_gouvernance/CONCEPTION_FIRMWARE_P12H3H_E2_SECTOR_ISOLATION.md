# P12-H3h-E2 — Isolation sectorielle du préfixe durable bulk

## Statut

CONCEPTION — amendement explicite de D4-C / D5-B / D5-C avant composition
physique avec E1.

Baseline d'entrée :

```text
45c3709e625e1512dc1e0a425689192c298daeab
Firmware: validate H3h-E1 SDMMC bulk media
```

## 1. Problème démontré par la composition réelle

D5-A encode actuellement un bloc compact :

```text
32 octets header + payload + 4 octets CRC
```

D5-B avance son offset physique de cette taille encodée exacte. D5-C utilise
l'extent physique ainsi calculé pour placer les blocs et campagnes suivants.

E1 traduit correctement une écriture byte-addressed partielle en
read-modify-write d'un secteur SD de 512 octets.

Conséquence : si un bloc D5 durable se termine au milieu d'un secteur, une
écriture ultérieure dans le même secteur peut réécrire par RMW des octets du
préfixe déjà durable. Une coupure pendant cette réécriture pourrait donc
contaminer un préfixe publié par un checkpoint antérieur.

Cette composition n'est pas compatible avec l'invariant P8-B selon lequel une
queue non checkpointée ne doit pas contaminer le préfixe durable précédent.

## 2. Règle E2

La granularité d'isolation physique du bulk V1 est 512 octets.

Tout bloc physique D5 :

1. commence à un offset multiple de 512 ;
2. conserve son encodage D5-A compact inchangé ;
3. occupe un extent physique égal à la taille encodée arrondie au multiple
   supérieur de 512 ;
4. réserve le padding restant jusqu'à la frontière suivante ;
5. fait avancer writer et reader de cet extent arrondi.

Le padding ne fait partie ni du payload logique, ni du CRC D5-A, ni de
`durable_prefix_bytes`.

## 3. Exemple minimal

Pour un payload logique de 16 octets :

```text
taille encodée = 32 + 16 + 4 = 52 octets
extent physique = 512 octets
```

Avec `data_base = 8192` :

```text
8192..8243  bloc D5 encodé
8244..8703  padding réservé
8704        prochain bloc possible
```

Une écriture du prochain bloc ne peut donc plus provoquer un RMW du secteur
contenant le bloc durable précédent.

## 4. Codec D5-A

D5-A reste inchangé.

Le CRC continue de couvrir uniquement header + payload. Le codec n'a pas à
connaître la granularité du média.

L'isolation sectorielle appartient au stream physique D5-B et à sa composition
D5-C.

## 5. Writer D5-B

L'initialisation du writer exige un `initial_offset` multiple de 512.

Avant écriture :

- calcul de la taille encodée D5-A ;
- calcul sûr de l'extent physique arrondi à 512 ;
- vérification de capacité sur l'extent physique complet.

L'écriture utile reste limitée aux octets encodés D5-A. L'extent jusqu'à la
frontière suivante est néanmoins réservé : aucune écriture D5 ultérieure ne
peut l'utiliser.

Après succès :

```text
next_offset += physical_extent
```

## 6. Reader D5-B

L'initialisation du reader exige également un offset multiple de 512.

Le reader :

- lit et valide l'encodage D5-A exact ;
- calcule le même extent physique arrondi ;
- avance de cet extent, sans interpréter le padding.

Le recovery ne dépend donc pas du contenu du padding.

## 7. D5-C

La zone data commence à `TR2_CAMPAIGN_BULK_METADATA_BYTES = 8192`, déjà
multiple de 512.

Les extents récupérés par D5-B étant sector-alignés, le `data_base` calculé
pour toute nouvelle campagne reste sector-aligné.

D5-C doit rejeter comme corrompu tout descriptor récupéré dont `data_base`
n'est pas multiple de 512.

## 8. Propriété de checkpoint

Après finalisation du dernier bloc d'un checkpoint et `sync()`, le prochain
offset writer se trouve sur un secteur distinct.

La publication D4-B peut alors autoriser le nouveau préfixe durable sans qu'une
écriture de queue ultérieure ait besoin de réécrire un secteur appartenant à
ce préfixe.

E2 ne revendique pas l'atomicité d'un secteur SD. Il supprime la dépendance à
cette atomicité entre un préfixe durable antérieur et une queue future.

## 9. Compatibilité / changement de layout

E2 modifie le placement physique des blocs D5-B/D5-C par rapport à la première
implémentation D5-C byte-packed.

Aucune migration automatique de médias expérimentaux antérieurs n'est définie.
Le backend n'étant pas encore branché au runtime de production, cette tranche
corrige le layout avant qualification physique de la composition finale.

## 10. Validation requise

Avant gel E2 :

- writer : premier bloc court avance exactement de 512 ;
- writer : bloc >512 avance au multiple de 512 suivant ;
- reader : retrouve deux blocs séparés par padding ;
- offsets init non alignés rejetés ;
- capacité vérifiée sur l'extent réservé complet ;
- D5-C : campagne suivante commence sur frontière 512 ;
- D5-C : recovery conserve le préfixe exact ;
- test explicite qu'une queue post-checkpoint écrit à partir d'un secteur
  différent de celui du dernier bloc durable ;
- suite complète et cross-build STM32 verts.

## 11. Non-objectifs

E2 ne :

- change pas le record P8 de 16 octets ;
- change pas le codec/CRC D5-A ;
- choisit pas la taille de payload de production ;
- résout pas les stalls SD ou le découplage acquisition/écriture ;
- branche pas encore D5-C au runtime STM32 ;
- publie pas encore la capacité dans `ConfigurationValidationEnvironment`.
