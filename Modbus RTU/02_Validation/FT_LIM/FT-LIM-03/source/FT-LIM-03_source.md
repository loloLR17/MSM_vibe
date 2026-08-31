# FT-LIM-03 — Source des exigences de validation

## 1. Référentiel

Sources normatives principales :
- Bloc 4 — Configuration acquisition ;
- Bloc 5 — Commandes, uniquement pour la commande code 1 « appliquer configuration préparée » et les résultats normatifs associés.

## 2. Exigences dérivées

### LIM03-RQ-001 — Algorithme CRC préparé

Le CRC préparé utilise CRC-32 / IEEE 802.3 : width 32, poly 0x04C11DB7, init 0xFFFFFFFF, refin true, refout true, xorout 0xFFFFFFFF.

### LIM03-RQ-002 — Périmètre CRC préparé

`prepared_config_crc` couvre les offsets 16 à 99 du Bloc 4, soit les zones 4B+4C+4D, en ordre croissant des registres.

### LIM03-RQ-003 — Sérialisation CRC

Chaque registre est injecté MSB puis LSB ; les uint32 sont exposés MSW puis LSW ; ASCII et registres réservés couverts sont inclus tels quels.

### LIM03-RQ-004 — Vecteur normatif

Le vecteur normatif n°1 défini par la V1 doit produire `0x5207CCFC`. Une implémentation ne peut être considérée validée si ce vecteur échoue.

### LIM03-RQ-005 — Recalcul firmware à l'application

Lors de la commande d'application, le firmware doit recalculer indépendamment le CRC de 4B+4C+4D et le comparer à `prepared_config_crc`.

### LIM03-RQ-006 — CRC incohérent

Si le CRC préparé ne correspond pas à l'image préparée, la configuration ne doit pas devenir active. L'image active et son CRC doivent rester inchangés.

### LIM03-RQ-007 — Modification préparée

Toute modification d'un champ de 4B+4C+4D invalide implicitement un état `VALIDE` précédent et repositionne la configuration en `BROUILLON`.

### LIM03-RQ-008 — Validation réussie

Une configuration préparée complète, cohérente et conforme aux domaines/contraintes normatifs peut être validée. La machine d'état prévoit `BROUILLON -> VALIDE` sur validation réussie.

### LIM03-RQ-009 — Validation échouée

Une configuration préparée invalide doit conduire à `ERREUR_VALIDATION` lorsqu'une validation est effectivement tentée. La configuration ne doit pas être activée.

### LIM03-RQ-010 — Correction après erreur de validation

Après `ERREUR_VALIDATION`, la modification d'au moins un champ préparé ramène la configuration à `BROUILLON`.

### LIM03-RQ-011 — Application réussie

Une configuration en état validable/`VALIDE`, appliquée avec succès via la commande Bloc 5 code 1, devient `ACTIF` et son image active 4E doit refléter de manière cohérente la configuration appliquée.

### LIM03-RQ-012 — Application interdite sans validation

Une configuration ne peut pas être appliquée si elle n'est pas validée. Il n'existe pas de transition directe normative `VIDE -> ACTIF`.

### LIM03-RQ-013 — Application échouée

Une application échouée depuis `VALIDE` conduit à `ERREUR_APPLICATION`. La nouvelle configuration ne doit pas remplacer l'image active précédente.

### LIM03-RQ-014 — Correction après erreur d'application

Après `ERREUR_APPLICATION`, la modification d'au moins un champ préparé ramène la configuration à `BROUILLON`.

### LIM03-RQ-015 — Nouvelle préparation depuis ACTIF

Depuis `ACTIF`, la préparation d'une nouvelle configuration distincte conduit à `BROUILLON` pour la préparation en cours, sans modifier immédiatement l'image active.

### LIM03-RQ-016 — Absence d'effet immédiat

L'écriture de la zone préparée n'a aucun effet immédiat sur l'acquisition ni sur l'image active. L'activation passe obligatoirement par la commande Bloc 5 code 1.

### LIM03-RQ-017 — Cohérence de l'image active

L'image 4E doit être cohérente et figée comme référence active. Une application réussie met à jour l'image active de manière cohérente ; un échec ne doit produire aucune mise à jour partielle.

### LIM03-RQ-018 — active_config_crc

`active_config_crc` est calculé par le firmware à partir de l'image active 4E et mis à jour lors de toute modification de cette image.

### LIM03-RQ-019 — Codes résultat utilisables

Le Bloc 5 définit notamment :
- résultat 4 : configuration invalide ;
- résultat 20 : configuration préparée incomplète.

Ces codes peuvent être exigés uniquement lorsque la cause testée correspond sans ambiguïté à leur définition normative.

### LIM03-RQ-020 — config_error_code non détaillé

Aucune table normative détaillée de `config_error_code` n'est fournie par la V1. FT-LIM-03 peut enregistrer sa valeur observée mais ne fixe aucun code attendu inventé.

### LIM03-RQ-021 — Observabilité de VALIDE

FT-LIM-03 vérifie la logique `BROUILLON -> VALIDE -> ACTIF`, mais n'impose pas de durée minimale d'observabilité Modbus de l'état `VALIDE` si validation et application sont enchaînées atomiquement par l'implémentation. La preuve porte sur le respect de la logique et l'absence de transition interdite.

## 3. Frontières

FT-LIM-03 suppose les domaines unitaires et contraintes croisées déjà couverts par FT-LIM-01/02. Les propriétés génériques de `submit`, `transaction_id` et idempotence restent hors périmètre, sauf comme préconditions d'utilisation correcte de la commande 1.
