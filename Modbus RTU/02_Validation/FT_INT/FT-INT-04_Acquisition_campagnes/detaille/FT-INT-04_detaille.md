# FT-INT-04 — Procédures détaillées

## Préconditions communes

- capteur dans un état maîtrisé ;
- configuration active valide lorsque START doit réussir ;
- stockage exploitable et capacité suffisante ;
- absence de défaut critique bloquant ;
- accès aux Blocs 0, 1, 2, 4, 5 et 6 selon les scénarios ;
- exécution des commandes B5 selon la procédure FT-CMD applicable ;
- pour FT-INT, une commande B5 n'est utilisée comme stimulus que lorsqu'elle est confirmée terminée avec succès.

Un défaut du moteur de commande ne doit pas être reclassé en défaut FT-INT.

---

## TT-INT-B01B05-001 — Activation effective de l'acquisition

### Objectif
Démontrer qu'un START B5 réussi produit l'effet inter-blocs attendu dans B1.

### Précondition spécifique
`B1.acquisition_state = 0` avant le stimulus.

### Procédure
1. Lire et consigner `B1.acquisition_state`.
2. Exécuter B5 commande `3 — démarrer acquisition` avec une transaction nouvelle.
3. Établir que la commande B5 est terminée avec succès selon FT-CMD.
4. Lire B1 après la fin de commande.
5. Consigner `acquisition_state`.

### Oracle
PASS si, après succès de START :
- `B1.acquisition_state = 1` (« en cours »).

FAIL si la commande est confirmée réussie mais que B1 reste dans un état incompatible avec une acquisition démarrée.

INCONCLUSIVE si le succès B5 n'est pas établi.

### Exclusions
Ne pas revalider les codes de refus, `submit`, `transaction_id` ou l'idempotence.

---

## TT-INT-B05B06-001 — Ouverture d'une nouvelle campagne au démarrage

### Objectif
Démontrer qu'un START réussi ouvre une nouvelle campagne B6.

### Procédure
1. Établir un état de référence de l'inventaire B6 avant START : campagnes accessibles, identifiants et états utiles.
2. Exécuter START B5 et confirmer son succès.
3. Parcourir l'inventaire B6 après START avec les mécanismes normatifs de sélection.
4. Identifier une campagne nouvellement ouverte correspondant au démarrage observé.
5. Conserver les éléments de preuve : `campaign_id`, état, timestamps et contexte temporel.

### Oracle
PASS si une nouvelle campagne correspondant au START réussi est démontrée dans B6.

FAIL si le START est confirmé réussi mais qu'aucune nouvelle campagne ne peut être établie après stabilisation raisonnable de l'inventaire.

INCONCLUSIVE si le banc ne permet pas de distinguer sans ambiguïté l'inventaire avant/après.

### Anti-surspécification
Ne pas imposer :
- un index précis pour la nouvelle campagne ;
- `total_campaign_count + 1` dès la première lecture ;
- l'égalité avec `B1.active_campaign_id`.

---

## TT-INT-B05B06-002 — Cohérence de la campagne en cours

### Objectif
Vérifier que la campagne ouverte par START est exposée comme campagne en cours selon les règles B6.

### Précondition
La campagne créée par `TT-INT-B05B06-001` est identifiable.

### Procédure
1. Sélectionner dans B6 la campagne attribuée au START.
2. Vérifier que la sélection est valide.
3. Lire `campaign_state` et `end_timestamp` dans un snapshot cohérent.
4. Consigner `campaign_id` comme trace, sans comparaison obligatoire à B1.

### Oracle
PASS si :
- `campaign_state = 2` (« en cours ») ;
- `end_timestamp = 0`.

FAIL si la campagne attribuée au START réussi est exposée dans un état incompatible avec une campagne en cours.

INCONCLUSIVE si la campagne ne peut pas être identifiée sans ambiguïté.

### Propriété
L'invariant B6 isolé `end_timestamp = 0` pour une campagne en cours reste propriétaire FT-BLK-05 ; FT-INT-04 l'utilise ici comme preuve de l'effet START→B6.

---

## TT-INT-B01B05B06-001 — Arrêt acquisition et clôture de campagne

### Objectif
Démontrer qu'un STOP réussi arrête B1 et met en cohérence la campagne B6 précédemment ouverte.

### Préconditions
- acquisition active ;
- campagne en cours identifiée avant STOP.

### Procédure
1. Consigner `B1.acquisition_state` et les métadonnées de la campagne en cours B6.
2. Exécuter B5 commande `4 — arrêter acquisition`.
3. Confirmer le succès de la commande selon FT-CMD.
4. Relire B1.
5. Resélectionner la campagne identifiée avant STOP dans B6.
6. Lire son état, `start_timestamp`, `end_timestamp` et `duration_s`.

### Oracle
PASS si :
- `B1.acquisition_state = 0` ;
- la campagne précédemment en cours n'est plus exposée comme `campaign_state = 2` ;
- `end_timestamp` n'est plus la valeur sentinelle d'une campagne en cours et les métadonnées sont cohérentes avec une campagne clôturée.

FAIL si STOP est confirmé réussi mais que B1 reste en acquisition ou que la campagne demeure exposée comme en cours.

INCONCLUSIVE si la campagne pré-STOP ne peut pas être retrouvée de façon déterministe.

### Limite
Ne pas imposer une égalité universelle exacte sur `duration_s` au-delà de la norme V1.

---

## TT-INT-B02B06-001 — Base temporelle des campagnes

### Objectif
Démontrer que les timestamps B6 sont exprimés dans la base temporelle B2.

### Procédure
1. Lire B2 immédiatement avant START et consigner une borne temporelle `T0`.
2. Exécuter START avec succès.
3. Lire B2 après START et consigner `T1`.
4. Identifier la campagne créée et relever `start_timestamp`.
5. Exécuter STOP avec succès.
6. Lire B2 autour de la fin de commande pour obtenir une fenêtre temporelle bornée.
7. Relever `end_timestamp` de la campagne clôturée.

### Oracle
PASS si les timestamps B6 sont cohérents avec la base B2 et se situent dans les fenêtres temporelles compatibles avec les événements observés, compte tenu des lectures successives.

FAIL si les timestamps sont incompatibles avec la base temporelle B2 ou avec l'ordre temporel des événements.

INCONCLUSIVE si les lectures B2 ne permettent pas d'encadrer les événements de manière exploitable.

### Anti-surspécification
Ne pas exiger l'égalité bit-à-bit entre `start_timestamp`/`end_timestamp` et une lecture séparée de B2.

---

## TT-INT-B00B04B05B06-001 — Conservation identité/configuration/campagnes après RAZ statistiques

### Objectif
Démontrer que la commande B5 `11 — RAZ statistiques` ne supprime ni les campagnes, ni l'identité capteur, ni la configuration.

### Préconditions
- au moins une campagne B6 valide et identifiable ;
- identité B0 lisible ;
- configuration B4 active identifiable ;
- acquisition arrêtée si requis par la procédure d'exécution retenue ;
- clé de confirmation correcte selon FT-CMD.

### Procédure
1. Relever une empreinte de référence de l'identité B0 suffisante pour détecter un effacement.
2. Relever l'identité/état de la configuration active B4.
3. Construire une empreinte de référence de l'inventaire B6 : campagnes et identifiants suffisants pour retrouver les entrées existantes.
4. Exécuter avec succès B5 commande `11`.
5. Relire B0 et vérifier que l'identité n'a pas été effacée.
6. Relire B4 et vérifier que la configuration n'a pas été effacée.
7. Rebalayer B6 et vérifier que les campagnes préexistantes sont toujours présentes et accessibles.

### Oracle
PASS si l'identité B0, la configuration B4 et les campagnes B6 préexistantes sont conservées après la RAZ statistiques.

FAIL si l'un de ces éléments est effacé du fait de la commande 11.

INCONCLUSIVE si les états de référence ne permettent pas d'établir la conservation de façon déterministe.

### Délégation
- validation de la clé, statuts et refus : FT-CMD ;
- persistance après reboot : FT-PER.

---

## Relations tracées sans PASS/FAIL actif

### `B1.active_campaign_id` ↔ `B6.campaign_id`
`NOT_DEFINED` : les deux champs existent mais leur égalité n'est pas explicitement imposée en V1.

### START ↔ incrément immédiat de `total_campaign_count`
`NOT_DEFINED` : l'ouverture d'une campagne est normative, mais la règle exacte de publication/incrément observable n'est pas formalisée.

### Refus START/STOP
`DELEGATED` à FT-CMD.