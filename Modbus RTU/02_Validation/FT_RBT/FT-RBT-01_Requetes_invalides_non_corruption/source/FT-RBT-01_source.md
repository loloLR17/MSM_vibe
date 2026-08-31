# FT-RBT-01 — Exigences source normalisées

## 1. Références normatives

- `01_Specification_source/charte_typage.md`, §14 — gestion des accès invalides ;
- spécifications normatives des blocs 0 à 7 lorsqu'elles imposent un rejet d'écriture et l'absence de modification ;
- `02_Validation/plan_test_modbus_tr2_squelette.md`, doctrine gelée des accès invalides et niveau E robustesse ;
- `02_Validation/FT_ACC/` pour la propriété de qualification des accès et exceptions.

Les compléments métier informatifs ne servent pas d'oracle.

## 2. Exigences et points de couverture

### RBT01-R01 — Qualification de l'accès invalide
- Source : charte de typage §14 et FT-ACC gelée.
- Règle : adresse inexistante, écriture sur RO, écriture sur registre réservé ou accès non autorisé par le mapping constituent des accès invalides.
- Classification : `DELEGATED`.
- Propriétaire : FT-ACC.
- Justification : FT-RBT ne redéfinit ni la validité de l'accès ni l'exception attendue.

### RBT01-R02 — Absence d'effet de bord de la requête rejetée
- Source : charte de typage §14 ; plan maître §2.5.
- Règle : une requête rejetée ne doit modifier aucun registre ni état interne et ne doit pas être exécutée partiellement.
- Classification : `DELEGATED` pour l'oracle élémentaire.
- Propriétaire : FT-ACC.
- Usage FT-RBT : cette garantie est utilisée comme jalon dans le scénario perturbé.

### RBT01-R03 — Requête invalide intercalée sans corruption observable du fonctionnement nominal
- Sources composées : charte de typage §14 + oracle nominal de l'échange valide choisi.
- Exigence de scénario : après établissement d'un état de référence, l'injection d'un accès invalide ne doit pas créer une modification observable interdite ; un échange valide ultérieur doit pouvoir être jugé avec exactement le même oracle nominal qu'en l'absence de la perturbation.
- Classification : `COVERED`.
- Test : `TT-RBT-GEN-001`.
- Nature : propriété FT-RBT par composition, sans création d'un nouveau timeout, état de récupération ou code résultat.

### RBT01-R04 — Temps maximal de récupération après accès invalide
- Source : aucune exigence V1 identifiée.
- Classification : `NOT_DEFINED`.
- Interdiction : ne pas imposer un délai, un nombre de cycles ou un nombre de requêtes avant retour au nominal.

### RBT01-R05 — Rafale d'accès invalides / seuil de tolérance
- Source : aucune exigence V1 identifiée.
- Classification : `NOT_DEFINED`.
- Interdiction : ne pas définir arbitrairement un nombre de requêtes invalides, une fréquence ou un seuil avant dégradation.

### RBT01-R06 — Reset, watchdog ou changement de mode après accès invalide
- Source : aucune règle V1 autorisant un tel comportement n'a été identifiée.
- Classification : `NOT_DEFINED` dans FT-RBT-01.
- Frontière : tout comportement post-reboot relève de FT-PER lorsqu'il est spécifié.

## 3. Instanciations admises pour TT-RBT-GEN-001

Le scénario peut être instancié avec une perturbation dont l'invalidité est déjà déterministe selon FT-ACC, par exemple :
- écriture sur un registre strictement RO ;
- écriture sur un registre réservé ;
- accès à une adresse inexistante.

Le choix d'une instance ne modifie pas l'oracle FT-RBT. Le code d'exception exact reste vérifié par FT-ACC et ne doit pas devenir un critère propriétaire FT-RBT.

## 4. Anti-duplication

FT-RBT-01 ne couvre pas :
- la table des permissions ;
- le code d'exception exact ;
- les domaines métier ;
- l'encodage des réponses ;
- la robustesse à un CRC de trame erroné ;
- la stratégie de retry de la centrale ;
- les scénarios de reboot.

## 5. Règles anti-fabrication

- ne pas assimiler une valeur métier hors domaine dans un registre RW à un accès Modbus invalide ;
- ne pas imposer de timeout de récupération ;
- ne pas exiger une séquence d'états internes après l'erreur ;
- ne pas extrapoler la règle d'accès invalide aux trames physiquement corrompues ;
- ne pas déclarer une rafale de N erreurs supportée sans source V1 ;
- ne pas transformer l'absence de corruption en obligation d'identité bit-à-bit pour des champs légitimement dynamiques entre deux lectures séparées.
