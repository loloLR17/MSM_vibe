# FT-SEQ-03 — Référentiel source : préparation et synchronisation temporelle

## 1. Objet

Identifier et valider la chaîne V1 complète par laquelle une centrale prépare une référence temporelle B2 puis l'applique effectivement au moyen de la commande B5 SYNC TIME.

FT-SEQ-03 porte la continuité de la chaîne. Les propriétés temporelles élémentaires, la transaction B5 et les effets B2 ↔ B5 restent aux familles gelées.

## 2. Sources normatives et délégations

Sources V1 principales :
- `01_Specification_source/bloc2.md` ;
- `01_Specification_source/bloc5.md`.

Oracles gelés composés :
- `FT_BLK/FT-BLK-02_Temps_monotonie_derivations/` : comportement interne B2, absence d'effet immédiat du temps préparé, monotonie et dérivations ;
- `FT_INT/FT-INT-01_Temps_commandes/` : application effective, `last_sync_time` et cohérence minimale de l'état temporel après succès ;
- `FT_CMD/FT-CMD-05_Configuration_temps/` : acceptation/refus et résultat SYNC TIME ;
- FT-STR / FT-ACC / FT-LIM pour structure, accès et domaines.

## 3. Chaîne normative retenue

La V1 établit les maillons suivants :
1. une référence temporelle est écrite dans la zone de temps préparé B2 ;
2. cette écriture ne modifie pas immédiatement l'horloge système ;
3. l'application effective passe par la commande B5 SYNC TIME ;
4. la commande exige notamment la présence d'un temps préparé et une horloge disponible ;
5. après succès, le temps préparé est effectivement appliqué à l'horloge B2 ;
6. `last_sync_time` est mis à jour lors de la synchronisation effective ;
7. les observables d'état temporel ne doivent pas rester contradictoires avec la synchronisation réalisée.

La V1 ne définit toutefois ni égalité bit-à-bit entre lectures séparées, ni tolérance chiffrée universelle, ni table exhaustive de tous les états/flags temporels.

## 4. Exigences FT-SEQ-03

### SEQ03-R01 — Préparer puis appliquer une synchronisation temporelle

- Classification : `COVERED`.
- Propriétaire : FT-SEQ-03.
- Test : `TT-SEQ-TIME-001`.
- Exigence : une référence temporelle préparée conformément à la V1, laissée sans effet immédiat sur l'horloge puis soumise à une commande SYNC TIME réussie, doit aboutir à une horloge et à des observables de synchronisation cohérents avec la référence préparée et le temps réellement écoulé.
- Nature de l'oracle FT-SEQ : continuité de la chaîne complète.

### SEQ03-R02 — Absence d'effet immédiat du temps préparé

- Classification : `DELEGATED`.
- Propriétaire : FT-BLK-02.
- Justification : propriété interne B2 déjà gelée ; FT-SEQ l'utilise comme jalon du scénario nominal.

### SEQ03-R03 — Acceptation et résultat SYNC TIME

- Classification : `DELEGATED`.
- Propriétaire : FT-CMD-05.
- Justification : la mécanique transactionnelle et les codes B5 ne sont pas réattribués à FT-SEQ.

### SEQ03-R04 — Application effective du temps préparé

- Classification : `DELEGATED`.
- Propriétaire : FT-INT-01.
- Justification : effet inter-blocs B5 → B2 déjà couvert.

### SEQ03-R05 — Mise à jour de last_sync_time

- Classification : `DELEGATED`.
- Propriétaire : FT-INT-01.

### SEQ03-R06 — Cohérence minimale de l'état temporel après succès

- Classification : `DELEGATED`.
- Propriétaire : FT-INT-01.
- Limite : aucune combinaison numérique exhaustive supplémentaire n'est créée.

### SEQ03-R07 — Égalité stricte current_time == prepared_time après commande

- Classification : `NOT_DEFINED`.
- Justification : les valeurs sont observées à des instants différents et le temps progresse. La V1 ne définit aucune égalité stricte ni tolérance chiffrée permettant un tel oracle.

### SEQ03-R08 — Égalité stricte last_sync_time == prepared_time lu avant commande

- Classification : `NOT_DEFINED` comme égalité bit-à-bit universelle entre transactions distinctes.
- Justification : l'oracle normatif est la cohérence avec la nouvelle référence temporelle appliquée, conformément à FT-INT-01, pas une égalité fabriquée hors contexte temporel.

### SEQ03-R09 — Délai maximal préparation → synchronisation

- Classification : `NOT_DEFINED`.
- Justification : aucune borne temporelle globale n'est définie.

### SEQ03-R10 — Équivalence exacte entre indicateur B5 de synchronisation préparée et états/flags B2

- Classification : `NOT_DEFINED`.
- Justification : FT-INT-01 a explicitement conservé cette relation comme non définie.

### SEQ03-R11 — Base temporelle commune de cmd_last_timestamp

- Classification : `TRACE_ONLY`.
- Propriétaire de la trace : FT-INT-01.
- Justification : base Epoch commune normative, mais absence de relation numérique précise imposable entre lectures séparées.

## 5. Oracle composé de TT-SEQ-TIME-001

Le scénario vérifie la continuité :

`horloge initiale` → `écriture temps préparé` → `pas de réglage immédiat` → `SYNC TIME réussi` → `horloge cohérente avec la nouvelle référence` → `last_sync_time mis à jour` → `état temporel non contradictoire`.

Le verdict FT-SEQ est PASS uniquement si tous les jalons applicables réussissent dans une même exécution.

La comparaison temporelle doit utiliser les oracles déjà définis par FT-BLK-02 et FT-INT-01. FT-SEQ n'ajoute ni tolérance arbitraire ni exigence d'égalité stricte.

## 6. Anti-fabrication

Ne pas imposer :
- `current_time == prepared_time` lors d'une lecture postérieure ;
- une tolérance numérique inventée ;
- une égalité bit-à-bit entre `last_sync_time` et une valeur lue dans une autre transaction si l'oracle gelé ne l'impose pas ;
- une séquence détaillée de `cmd_status` ;
- un délai maximal global ;
- une équivalence exhaustive entre flags B2 et B5 ;
- une table d'états temporels plus précise que la V1.

## 7. Frontière avec FT-SEQ-07

Le scénario `SYNC TIME sans temps préparé → refus 19 → préparation du temps → nouvelle commande → succès` appartient à FT-SEQ-07. FT-SEQ-03 couvre le chemin nominal complet.