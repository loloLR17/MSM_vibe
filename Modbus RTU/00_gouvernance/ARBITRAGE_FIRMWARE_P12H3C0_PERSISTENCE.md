# P12-H3c0 — Arbitrage de persistance physique TR2

## 1. Objet

Déterminer si le layout persistant actuellement implémenté doit être dimensionné tel quel sur la cible STM32U575, ou si la politique de journalisation B5 doit être séparée du choix du média physique avant composition complète de `SystemRuntime`.

Cette tranche ne modifie ni le mapping Modbus V1 ni les sémantiques transactionnelles B5.

## 2. Faits établis par le code courant

Le `CommandJournalStore` adresse directement deux slots redondants de 66 octets pour chaque `transaction_id` valide.

```text
slot_offset =
  ((transaction_id - 1) * 2 + slot_index) * 66
```

La constante courante est :

```text
TR2_COMMAND_JOURNAL_STORE_MAX_TRANSACTION_ID = UINT16_MAX
```

Le journal réserve donc :

```text
65 535 * 2 * 66 = 8 650 620 octets
```

Le layout persistant total calculé par H3a atteint 8 666 598 octets.

Le linker STM32 courant déclare 2 MiB de FLASH interne. Le layout courant ne peut donc pas être transposé intégralement dans cette FLASH.

## 3. Contrainte normative B5

La spécification V1 impose :

- `transaction_id = 0` invalide ;
- `1..65535` valides ;
- une commande avec un `transaction_id` déjà traité ne doit jamais être exécutée une seconde fois ;
- le résultat précédent doit être réutilisé pour un identifiant déjà traité ;
- la politique de réutilisation d'un identifiant **après disparition de son historique d'idempotence n'est pas définie par la V1**.

Cette dernière règle est déterminante : la V1 ne définit pas une durée de rétention minimale ni une politique de recyclage permettant aujourd'hui de réduire arbitrairement le journal.

## 4. Observation architecturale

Le besoin de 8,65 Mo n'est pas un besoin intrinsèque du protocole Modbus. Il résulte de l'implémentation actuelle d'une table dense indexée directement par les 65 535 valeurs possibles de `transaction_id`.

Cette organisation apporte une propriété simple : tant que le journal n'est pas explicitement perdu, chaque identifiant dispose d'un emplacement persistant stable et peut être retrouvé après reboot.

Elle entraîne en contrepartie :

- une capacité fixe de 8,65 Mo ;
- un scan de 65 535 identifiants au recovery ;
- un couplage fort entre domaine numérique du `transaction_id` et capacité physique ;
- des écritures répétées sur deux records alternés par transaction.

## 5. Options examinées

### Option A — Conserver le journal dense et ajouter un média externe >= layout requis

Avantages :
- aucun changement des sémantiques B5 ;
- aucune modification de `CommandJournalStore` ;
- conservation directe de l'idempotence pour tous les IDs présents.

Inconvénients :
- impose un composant de stockage supplémentaire ;
- endurance, atomicité, power-loss et interface physique restent à qualifier ;
- recovery par scan complet conservé ;
- choix matériel prématuré tant que le besoin de rétention n'est pas normativement fixé.

### Option B — Réduire `TR2_COMMAND_JOURNAL_STORE_MAX_TRANSACTION_ID`

Rejetée pour la V1 courante.

Cela rendrait une partie des IDs 1..65535 non journalisables alors que la spécification les définit comme valides. Ce serait une modification de comportement normative, pas un simple choix de stockage.

### Option C — Journal borné/circulaire avec éviction

Non retenue sans évolution normative préalable.

Une éviction introduit précisément la notion de « disparition de l'historique d'idempotence ». La V1 indique que la politique de réutilisation après cette disparition n'est pas définie. Choisir une profondeur ou une durée de rétention ici inventerait donc une politique absente de la V1.

### Option D — Structure persistante clairsemée/indexée

Architecturalement possible : ne stocker que les transactions effectivement utilisées, avec index/recherche et stratégie de compaction.

Elle pourrait réduire fortement l'espace réellement consommé sans changer le domaine 1..65535.

Cependant elle exige une nouvelle conception de persistance avec garanties de :
- lookup déterministe ;
- détection des collisions ;
- atomicité power-loss ;
- conservation des résultats terminaux ;
- récupération des transactions incomplètes ;
- absence de réexécution après reboot ;
- gestion de la capacité pleine.

Ce n'est pas une modification à introduire implicitement dans H3.

## 6. Décision P12-H3c0

**Ne pas choisir maintenant un composant mémoire externe uniquement pour satisfaire le layout dense actuel.**

**Ne pas réduire le domaine des transaction IDs et ne pas introduire d'éviction tant que la politique de rétention/réutilisation B5 n'est pas explicitement arbitrée.**

Le backend `PersistentMedia` STM32 reste donc ouvert.

La prochaine tranche doit être :

**P12-H3c1 — politique de rétention et de réutilisation des transaction IDs B5.**

Cette tranche devra décider explicitement ce que signifie « déjà traité » dans la durée de vie du capteur et dans quelles conditions, s'il y en a, un ID peut redevenir admissible.

Seulement après cette décision, on pourra comparer honnêtement :
- journal dense + mémoire externe ;
- journal sparse/indexé ;
- journal borné avec politique normative de recyclage.

## 7. Conséquences immédiates

Jusqu'à H3c1 :

- `TR2_COMMAND_JOURNAL_STORE_MAX_TRANSACTION_ID` reste inchangé ;
- `CommandJournalStore` reste inchangé ;
- `PersistentMedia` STM32 n'est pas implémenté ;
- aucune mémoire externe n'est sélectionnée ;
- `SystemRuntime` complet n'est pas raccordé dans `main.c`.

## 8. Invariants préservés

- B0..B7 inchangés ;
- domaine B5 `transaction_id = 1..65535` inchangé ;
- idempotence inchangée ;
- récupération power-loss inchangée ;
- aucun retry automatique ;
- aucun stub de persistance ;
- aucune validation hardware revendiquée.
