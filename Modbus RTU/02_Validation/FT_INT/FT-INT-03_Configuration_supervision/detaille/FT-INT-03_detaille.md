# FT-INT-03 — Procédures détaillées B4↔B3

## 1. Préconditions générales

- disposer d'une configuration B4 active A valide ;
- disposer d'un moyen de lire de façon cohérente le snapshot B3 ;
- pour les tests dépendant d'un niveau vibratoire déterminé, disposer d'un banc d'injection, d'un simulateur ou d'un rejeu déterministe ;
- toute application de configuration B4 est effectuée via la procédure déjà validée par FT-INT-02 / FT-CMD ; la mécanique B5 n'est pas un objet de verdict ici.

Si le niveau vibratoire appliqué n'est pas suffisamment connu ou reproductible pour déterminer le verdict attendu, le test concerné est `INCONCLUSIVE`, pas `FAIL`.

## 2. TT-INT-B03B04-001 — Seuil actif versus seuil préparé

### Objet

Démontrer que B3 utilise la configuration B4 active et non une configuration seulement préparée.

### Préparation

1. Activer une configuration A avec des seuils connus.
2. Préparer une configuration B distincte dont au moins un seuil de supervision pertinent diffère de A.
3. Ne pas appliquer B.
4. Choisir un stimulus déterministe permettant de distinguer sans ambiguïté le verdict attendu avec A de celui attendu avec B.

### Exécution

1. Vérifier que A reste la configuration active.
2. Appliquer le stimulus déterministe pendant une fenêtre complète.
3. Attendre une fenêtre B3 validée.
4. Lire le snapshot B3 et les champs de décision pertinents explicitement exploitables.

### Oracle

`PASS` si le résultat est compatible avec les seuils de A et incompatible avec l'utilisation prématurée des seuils préparés de B.

`FAIL` si une décision B3 ne peut s'expliquer que par l'utilisation de B alors que B n'est pas active.

`INCONCLUSIVE` si le stimulus ne permet pas de discriminer A de B ou si la fenêtre n'est pas validée.

## 3. TT-INT-B03B04-002 — Absence de rétroactivité

### Objet

Vérifier qu'une nouvelle configuration ne réinterprète pas un résultat B3 déjà calculé et validé.

### Préparation

1. Avec la configuration active A, obtenir un snapshot B3 validé S_A.
2. Conserver les champs nécessaires à l'identification du snapshot, notamment `B3_CALC_SEQUENCE` et les résultats pertinents.
3. Préparer puis appliquer une configuration B différente selon la procédure dédiée.

### Exécution

1. Immédiatement après activation de B, relire B3 avant qu'une nouvelle fenêtre validée ne soit observée, si le rythme du système permet cette observation.
2. Identifier si `B3_CALC_SEQUENCE` correspond encore à S_A.

### Oracle

Si le même snapshot S_A est encore exposé, ses résultats ne doivent pas avoir été modifiés uniquement à cause de l'activation de B.

`PASS` si S_A reste inchangé tant qu'il reste le snapshot courant.

`FAIL` si, à séquence de calcul identique, des résultats déjà validés sont réinterprétés du seul fait du changement de configuration.

Si une nouvelle fenêtre validée remplace S_A avant toute lecture exploitable, le cas est `INCONCLUSIVE` et non un échec.

Aucune durée minimale de conservation du snapshot n'est imposée.

## 4. TT-INT-B03B04-003 — Prise d'effet à la prochaine fenêtre validée

### Objet

Démontrer que la nouvelle configuration active est utilisée à partir de la prochaine fenêtre B3 validée.

### Préparation

1. Activer une configuration A.
2. Préparer une configuration B avec un seuil distinct.
3. Appliquer B avec succès.
4. Utiliser un stimulus déterministe dont le résultat attendu sous B est connu.

### Exécution

1. Relever la séquence B3 avant la première nouvelle fenêtre post-activation.
2. Maintenir/appliquer le stimulus déterministe.
3. Attendre l'incrément correspondant à une nouvelle fenêtre validée.
4. Lire le nouveau snapshot B3.

### Oracle

`PASS` si la première fenêtre validée post-activation produit une décision compatible avec les seuils actifs de B.

`FAIL` si cette fenêtre produit encore une décision démontrant l'utilisation des anciens seuils A.

`INCONCLUSIVE` si le stimulus ou la validité de fenêtre ne permet pas un verdict déterministe.

## 5. TT-INT-B03B04-004 — Bascule discriminante de seuil

### Objet

Démontrer expérimentalement la bascule d'une décision B3 lors du passage d'une configuration active A à B, sans changer le stimulus.

### Construction du scénario

Choisir un indicateur et deux seuils A et B tels que le niveau vibratoire contrôlé V soit strictement situé entre eux. Exemple générique :

`seuil_A < V < seuil_B`

ou l'ordre inverse si cela facilite le banc.

Les valeurs choisies doivent rester dans les domaines V1 ; le test ne crée aucune nouvelle valeur normative.

### Exécution

1. Activer A.
2. Appliquer le stimulus V sur une fenêtre validée et relever le verdict B3.
3. Préparer puis appliquer B sans modifier V.
4. Vérifier qu'aucune réinterprétation rétroactive du snapshot précédent n'a lieu si celui-ci est encore exposé.
5. Attendre la prochaine fenêtre validée sous B.
6. Relever le nouveau verdict B3.

### Oracle

`PASS` si la décision bascule conformément au changement de seuil entre A et B à stimulus constant, et seulement à partir de la nouvelle fenêtre validée.

`FAIL` si la bascule survient avant activation, si elle est rétroactive sur un snapshot déjà validé, ou si la nouvelle fenêtre reste manifestement évaluée avec l'ancien seuil.

`INCONCLUSIVE` si V n'est pas suffisamment stable/connu pour garantir sa position entre les deux seuils.

## 6. Contrôles associés sans duplication

Les essais peuvent enregistrer à titre de trace :
- `B3_STATUS_GLOBAL` ;
- `B3_VALIDITY_FLAGS` ;
- `B3_ALARM_FLAGS` ;
- `B3_SEVERITY_GLOBAL` ;
- `B3_CALC_SEQUENCE` ;
- les indicateurs RMS/crête concernés ;
- l'identité de configuration B4 active.

Ces traces ne doivent pas être transformées en oracles exhaustifs si la V1 ne définit pas leur dérivation complète.

## 7. Critères de sortie FT-INT-03

La sous-famille est acceptable si :
- les relations B4→B3 normatives sont toutes tracées ;
- aucun test ne duplique FT-BLK, FT-LIM, FT-STR ou FT-CMD ;
- l'absence de banc déterministe produit `INCONCLUSIVE`/`CONDITIONAL` plutôt qu'un oracle inventé ;
- les zones `NOT_DEFINED` restent explicites.
