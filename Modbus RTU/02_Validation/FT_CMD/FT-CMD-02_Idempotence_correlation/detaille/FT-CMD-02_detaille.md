# FT-CMD-02 — Cas de test détaillés

## TT-CMD-B05-100 — Rejeu immédiat : réutilisation du résultat précédent

**Objectif** : vérifier l'oracle directement observable d'idempotence.

**Préconditions** :
- moteur de commande disponible ;
- choisir une commande autorisée et sûre dans le contexte d'essai ;
- choisir un `transaction_id = T` non utilisé dans la séquence immédiate ;
- `submit = 0` avant première soumission.

**Procédure** :
1. soumettre la commande avec `transaction_id = T` ;
2. attendre sa terminaison et relever au minimum `cmd_status` et `cmd_result_code` ;
3. conserver le résultat final observé ;
4. après retour de `submit` à `0`, soumettre de nouveau la même requête avec le même `transaction_id = T` ;
5. relire les champs de réponse.

**Oracle** :
- le rejeu est présenté comme terminé ;
- `cmd_result_code` réutilise le résultat de la première transaction ;
- aucun nouveau résultat fonctionnel distinct ne doit être produit pour ce même identifiant.

**Note de rigueur** : ce cas prouve la réutilisation du résultat. Il ne suffit pas, à lui seul, à prouver physiquement l'absence d'une seconde exécution interne si l'effet de la commande n'est pas discriminant.

---

## TT-CMD-B05-101 — Non-réexécution du même identifiant

**Classification d'exécution : `CONDITIONAL`.**

**Objectif** : démontrer qu'un `transaction_id` déjà traité n'est jamais exécuté une seconde fois.

**Précondition supplémentaire obligatoire** : disposer d'au moins un moyen d'observation discriminant, par exemple :
- instrumentation firmware / trace d'exécution dédiée ; ou
- effet métier explicitement normé permettant de distinguer une seconde exécution d'un rejeu sans ambiguïté.

**Procédure** :
1. exécuter une transaction `T` jusqu'à terminaison ;
2. relever l'observable discriminant avant rejeu ;
3. rejouer immédiatement la transaction `T` ;
4. relever à nouveau le même observable.

**Oracle** :
- aucune seconde exécution de l'action n'est observée ;
- le résultat précédent est réutilisé.

**Verdict** :
- `PASS` si l'observable discriminant démontre la non-réexécution ;
- `FAIL` si une seconde exécution est observée ;
- `NOT_EXECUTABLE` si aucune observation discriminante n'est disponible. Ne pas convertir `NOT_EXECUTABLE` en `PASS`.

---

## TT-CMD-B05-102 — Même code, nouvel identifiant : nouvelle transaction

**Objectif** : vérifier la distinction entre rejeu et nouvelle transaction.

**Préconditions** :
- choisir une commande autorisée pouvant être soumise deux fois sans danger dans le contexte d'essai ;
- choisir deux identifiants distincts `T1` et `T2`.

**Procédure** :
1. soumettre la commande avec `T1` et attendre sa terminaison ;
2. remettre en place les préconditions nécessaires si la commande les modifie ;
3. soumettre le même code commande avec `T2` ;
4. lire `cmd_active_transaction_id` et le résultat de la seconde transaction.

**Oracle** :
- la seconde soumission n'est pas traitée comme le rejeu de `T1` ;
- `cmd_active_transaction_id = T2` pour la transaction prise en compte ;
- le résultat est celui de l'évaluation de la nouvelle transaction `T2`, sans obligation qu'il soit identique à celui de `T1`.

---

## TT-CMD-B05-103 — Corrélation nominale de réponse

**Objectif** : vérifier l'exposition de l'identifiant permettant la corrélation stricte requête/réponse.

**Préconditions** :
- choisir un `transaction_id = T` ;
- disposer d'une commande admissible dans le contexte courant.

**Procédure** :
1. soumettre la commande avec `T` ;
2. lire `cmd_active_transaction_id`, `cmd_status` et `cmd_result_code` ;
3. ne considérer les champs de statut/résultat comme réponse à `T` que lorsque l'identifiant exposé correspond à `T`.

**Oracle capteur** :
- la transaction prise en compte est exposée avec `cmd_active_transaction_id = T`.

**Oracle côté centrale** :
- une lecture avec `cmd_active_transaction_id != T` ne doit pas être interprétée comme la réponse à la requête `T`.

**Frontière** : le cas concurrent où une autre commande est déjà active appartient à FT-CMD-04 ; ce test est strictement nominal.

---

## Cas non instanciés faute d'oracle V1

### Profondeur de mémoire d'idempotence

Aucun test de type « rejouer après N autres transactions » ou « rejouer après X minutes/heures » n'est normatif : `N` et `X` ne sont pas définis.

### Même identifiant, contenu différent

Aucun verdict n'est fixé pour un rejeu de `transaction_id = T` avec un code ou des paramètres différents de ceux de la transaction originale. Ce conflit doit être précisé dans une future évolution de la spécification avant industrialisation du test.

### Reboot

La conservation ou non de la mémoire des transactions traitées après reboot n'est pas tranchée ici. Toute exigence correspondante relève de FT-PER et nécessite une source normative explicite.
