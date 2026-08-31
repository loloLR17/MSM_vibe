# FT-RBT-02 — Cas de test détaillés

## TT-RBT-B05-001 — Perte de réponse puis retransmission identique

**Classification d'exécution : `CONDITIONAL`.**

**Objectif**  
Vérifier qu'après traitement d'une transaction dont la première réponse est perdue, une retransmission immédiate de la même transaction permet de récupérer le résultat précédent sans créer une nouvelle transaction logique.

**Sources normatives**  
- Bloc 5 §1, §4.2, §7.2 ;
- FT-CMD-02 gelée.

**Préconditions**
- moteur de commande disponible ;
- aucune commande active ;
- choisir une commande sûre et admissible dans le contexte d'essai ;
- choisir un `transaction_id = T` non utilisé dans la séquence immédiate ;
- moyen d'essai capable de perdre/supprimer la première réponse tout en laissant la requête initiale parvenir au capteur et être traitée.

**Étapes**
1. soumettre une commande avec `transaction_id = T` ;
2. injecter la perte de la première réponse avant qu'elle soit remise à la centrale ;
3. confirmer, par instrumentation de banc ou observation indépendante autorisée, que la requête initiale a bien été traitée côté capteur ;
4. sans modifier le code ni les paramètres, retransmettre la même transaction `T` ;
5. lire la réponse disponible après retransmission ;
6. appliquer l'oracle de corrélation FT-CMD-02 pour vérifier que le résultat récupéré correspond à `T`.

**Résultat attendu**
- la retransmission de `T` ne doit pas être traitée comme une nouvelle transaction logique ;
- le résultat précédemment associé à `T` est réutilisé ;
- le résultat récupéré reste corrélable à `T` selon l'oracle FT-CMD-02.

**Limites**
- le test n'impose aucun délai maximal de retransmission ;
- l'utilisation d'une retransmission immédiate est un choix de construction du test, pas une exigence V1 ;
- ce test ne suffit pas, seul, à démontrer physiquement l'absence de seconde exécution si l'action n'a pas d'effet discriminant.

**Verdict**
- `PASS` si la perturbation est effectivement injectée, la première transaction est confirmée traitée, et la retransmission récupère le résultat précédent de `T` ;
- `FAIL` si une transaction logique distincte est observée pour le même `T` ou si le résultat précédent n'est pas réutilisé ;
- `NOT_EXECUTABLE` si la perte de réponse ne peut pas être injectée de façon déterministe.

---

## TT-RBT-B05-002 — Absence de double exécution après perte et retransmission

**Classification d'exécution : `CONDITIONAL`.**

**Objectif**  
Démontrer sous perturbation que la perte de la première réponse puis le rejeu de `T` ne provoquent pas une seconde exécution de l'action.

**Préconditions supplémentaires obligatoires**
- toutes les préconditions de `TT-RBT-B05-001` ;
- disposer d'un observable discriminant permettant de compter ou distinguer les exécutions réelles de l'action.

Cet observable peut être :
- une instrumentation firmware dédiée au banc ;
- ou un effet métier explicitement normé et réellement discriminant.

**Étapes**
1. relever l'observable discriminant avant la transaction ;
2. soumettre `T` et supprimer la première réponse ;
3. confirmer que l'action initiale a été exécutée une fois ;
4. retransmettre exactement `T` ;
5. récupérer le résultat de `T` ;
6. relever à nouveau l'observable discriminant.

**Résultat attendu**
- une seule exécution réelle de l'action est observée sur l'ensemble `soumission initiale + retransmission` ;
- la retransmission réutilise le résultat précédent.

**Verdict**
- `PASS` si une seule exécution réelle est démontrée ;
- `FAIL` si une seconde exécution est observée ;
- `NOT_EXECUTABLE` si aucun observable discriminant n'est disponible.

**Interdiction**  
Ne jamais assimiler la simple égalité des codes résultat à une preuve suffisante de non-réexécution.

---

## Cas volontairement non instanciés

### Plusieurs retries successifs

Aucun test normatif ne fixe 2, 3 ou N retransmissions. La V1 ne définit pas de politique ni de profondeur de retry.

### Retry après délai imposé

Aucun délai minimal/maximal n'est défini. Aucun test de type `attendre X ms/s puis rejouer` ne porte de verdict V1 sur la durée.

### Même transaction_id avec payload modifié

Cas `NOT_DEFINED` déjà possédé par FT-CMD-02. Il n'est pas transformé en scénario de robustesse.

### Rejeu après reboot

Hors FT-RBT-02. Relève de FT-PER et nécessite un oracle de persistance explicite.
