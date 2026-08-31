# FT-BLK-04 — Exigences source normalisées

## 1. Référence normative

Source principale : `01_Specification_source/bloc4.md` V1 gelé.

## 2. Exigences normalisées

### BLK04-B4-001 — Première préparation
Depuis `VIDE`, une écriture partielle ou initiale d'une configuration préparée conduit à `BROUILLON`.

**Classification** : COVERED.

### BLK04-B4-002 — Invalidation d'un état VALIDE
Toute modification d'un champ de la zone préparée invalide implicitement l'état `VALIDE` et repositionne la configuration en `BROUILLON`.

**Classification** : COVERED.

### BLK04-B4-003 — Reprise après ERREUR_VALIDATION
Depuis `ERREUR_VALIDATION`, la modification d'au moins un champ préparé conduit à `BROUILLON`.

**Classification** : COVERED.

### BLK04-B4-004 — Reprise après ERREUR_APPLICATION
Depuis `ERREUR_APPLICATION`, la modification d'au moins un champ préparé conduit à `BROUILLON`.

**Classification** : COVERED.

### BLK04-B4-005 — Nouvelle préparation depuis ACTIF
Depuis `ACTIF`, la préparation d'une nouvelle configuration distincte conduit à `BROUILLON`.

**Classification** : COVERED.

### BLK04-B4-006 — Pas de transition directe VIDE→ACTIF
Une configuration ne peut pas passer directement de `VIDE` à `ACTIF`.

**Classification** : COVERED comme invariant de cycle ; la tentative d'application concrète relève de FT-CMD.

### BLK04-B4-007 — Application interdite si non VALIDE
Une configuration ne peut pas être appliquée si elle n'est pas en état `VALIDE`.

**Classification** : DELEGATED à FT-CMD ; traçabilité conservée ici.

### BLK04-B4-008 — Validation réussie
`BROUILLON` → `VALIDE` après validation réussie.

**Classification** : DELEGATED à FT-CMD.

### BLK04-B4-009 — Validation échouée
`BROUILLON` → `ERREUR_VALIDATION` après validation échouée.

**Classification** : DELEGATED à FT-CMD.

### BLK04-B4-010 — Application réussie
`VALIDE` → `ACTIF` après commande d'application réussie.

**Classification** : DELEGATED à FT-CMD / FT-INT.

### BLK04-B4-011 — Application échouée
`VALIDE` → `ERREUR_APPLICATION` après commande d'application échouée.

**Classification** : DELEGATED à FT-CMD.

### BLK04-B4-012 — Absence d'effet immédiat de la préparation
La configuration préparée n'a aucun effet immédiat sur la configuration active.

**Classification** : COVERED.

### BLK04-B4-013 — Image active figée hors application
L'image active 4E reste la référence de la configuration appliquée et ne doit pas être modifiée par la seule préparation d'une nouvelle configuration.

**Classification** : COVERED.

### BLK04-B4-014 — CRC préparé : algorithme et périmètre
`prepared_config_crc` utilise CRC-32 / IEEE 802.3 sur les offsets 16 à 99, ordre croissant des registres, MSB puis LSB, zones réservées incluses.

**Classification** : COVERED.

### BLK04-B4-015 — Vecteur CRC normatif n°1
Le jeu de registres normatif défini par la V1 sur les offsets 16 à 99 doit produire `0x5207CCFC`.

**Classification** : COVERED, P0.

### BLK04-B4-016 — Responsabilité du prepared_config_crc
Après modification de 4B+4C+4D, la centrale recalcule et met à jour `prepared_config_crc`. La V1 n'impose pas que le firmware réécrive automatiquement ce champ à chaque modification.

**Classification** : TRACE_ONLY / garde-fou de non-surspécification.

### BLK04-B4-017 — Vérification firmware du CRC lors de l'application
Lors de la commande d'application, le firmware recalcule indépendamment le CRC préparé et le compare à `prepared_config_crc`.

**Classification** : DELEGATED à FT-CMD.

### BLK04-B4-018 — CRC actif
`active_config_crc` est calculé par le firmware à partir de l'image active 4E et mis à jour lors de toute modification de cette image.

**Classification** : CONDITIONAL en FT-BLK-04 lorsque l'image active peut être placée dans un état connu sans inventer de séquence B5 ; sinon exécution déléguée.

### BLK04-B4-019 — Compteur de révision
La V1 expose `config_revision_counter` sans préciser l'événement exact d'incrément.

**Classification** : NOT_DEFINED.

### BLK04-B4-020 — Code erreur de configuration
La V1 expose `config_error_code` sans fournir ici une table exhaustive reliant chaque erreur fonctionnelle à un code déterministe.

**Classification** : NOT_DEFINED pour la dérivation détaillée.

## 3. Exclusions

Les domaines des champs préparés, relations de bornes et compatibilités métier restent traités dans FT-LIM. Les commandes B5 restent traitées dans FT-CMD. Les effets de l'image active sur B3 relèvent de FT-INT.
