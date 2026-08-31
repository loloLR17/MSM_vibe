# FT-LIM-03 — Procédure d'exécution

## 1. Principe

Les instances actives sont listées dans `FT-LIM-03_instancie_index.csv`. Les procédures sont regroupées par cas générique afin d'éviter des fiches répétitives.

## 2. Baseline et instrumentation

Avant chaque essai, enregistrer au minimum :
- `config_state` ;
- `config_error_code` à titre d'observation ;
- `prepared_config_id` ;
- `prepared_config_crc` ;
- `active_config_id` ;
- `active_config_crc` ;
- image active complète 4E ;
- valeurs préparées 4B+4C+4D concernées ;
- résultat final de la commande Bloc 5 utilisée.

Les tests utilisant la commande 1 doivent employer une transaction neuve et respecter la procédure de soumission correcte du Bloc 5, sans revalider ici la doctrine générique de commande.

## 3. Tests CRC

### TT-LIM-03-001

Charger exactement le vecteur normatif n°1 et calculer indépendamment le CRC selon l'algorithme et la sérialisation V1. Le résultat doit être `0x5207CCFC`.

### TT-LIM-03-002

1. partir d'une préparation B valide avec CRC correct ;
2. modifier un registre dans les offsets 16..99 ;
3. conserver volontairement l'ancien `prepared_config_crc` ;
4. tenter l'application ;
5. vérifier le refus fonctionnel et la conservation intégrale de A.

### TT-LIM-03-003

Altérer seulement `prepared_config_crc` d'une préparation B inchangée et valide. Vérifier le même comportement de protection.

### TT-LIM-03-004

Modifier validement un champ préparé, recalculer correctement le CRC, puis appliquer avec toutes les autres préconditions satisfaites. Le CRC ne doit pas constituer une cause d'échec.

### TT-LIM-03-005 / 006

Calculer le CRC préparé avant/après modification :
- d'un registre couvert 4B+4C+4D : le CRC attendu change ;
- de `prepared_config_id` en 4A seulement : le CRC attendu des offsets 16..99 ne change pas.

## 4. Tests de machine d'état

### TT-LIM-03-007

Depuis une configuration préparée précédemment `VALIDE`, modifier un champ de 4B+4C+4D. Vérifier `BROUILLON`.

### TT-LIM-03-008

Préparer B complète et valide, avec CRC correct, puis demander l'application. Vérifier la séquence logique de validation puis l'état final `ACTIF`. Si `VALIDE` est observable, l'enregistrer ; son absence d'observation transitoire n'est pas un FAIL à elle seule.

### TT-LIM-03-009

Introduire une invalidité fonctionnelle déjà normativement couverte, recalculer le CRC afin que l'échec ne soit pas imputable au CRC, puis demander l'application. Vérifier `ERREUR_VALIDATION` et l'absence d'activation.

### TT-LIM-03-010

Construire une préparation dont l'incomplétude est sans ambiguïté selon la V1. Demander l'application. Vérifier `cmd_result_code = 20` et l'absence d'activation.

Ne pas utiliser une notion d'incomplétude inventée ou seulement métier.

### TT-LIM-03-011

Après TT-LIM-03-009, corriger au moins un champ de 4B+4C+4D. Vérifier le retour à `BROUILLON` avant toute nouvelle validation.

### TT-LIM-03-012

Depuis `VIDE` ou une préparation non validable, demander l'application. Vérifier que `ACTIF` n'est jamais atteint.

### TT-LIM-03-013 / 014

Ces cas nécessitent un moyen contrôlé de provoquer un échec après validation réussie. Si ce moyen existe :
- vérifier `VALIDE -> ERREUR_APPLICATION` ;
- vérifier ensuite qu'une modification de 4B+4C+4D ramène `BROUILLON`.

Sinon, documenter `N/A — APPLICATION_FAILURE_NOT_INJECTABLE` pour les deux cas.

### TT-LIM-03-015

Depuis A active, préparer B distincte sans l'appliquer. Vérifier `BROUILLON` et que 4E reste A.

## 5. Tests de cohérence active

### TT-LIM-03-016

Après succès d'application de B :
1. lire l'image 4E complète dans des conditions garantissant une lecture cohérente ;
2. vérifier que tous les champs reflètent B ;
3. recalculer indépendamment le CRC de 4E selon les règles V1 applicables ;
4. vérifier l'égalité avec `active_config_crc`.

### TT-LIM-03-017

Après un échec de validation/application de B :
1. relire 4E ;
2. comparer bit à bit/champ à champ au snapshot A ;
3. vérifier que `active_config_crc` est inchangé ;
4. vérifier qu'aucun sous-ensemble de B n'est apparu dans 4E.

## 6. Verdicts

PASS : tous les critères normatifs applicables sont satisfaits.

FAIL majeur notamment si :
- vecteur CRC normatif incorrect ;
- CRC faux accepté jusqu'à activation ;
- transition interdite vers ACTIF ;
- image active partiellement mise à jour ;
- `active_config_crc` incohérent ;
- image active précédente perdue après échec.

N/A : uniquement pour un cas conditionnel dont la précondition d'injection n'est pas disponible, avec justification documentée.
