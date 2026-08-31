# FT-LIM-03 — Cas génériques détaillés

## 1. Préconditions communes

Sauf mention contraire :

1. disposer d'une configuration active valide A ;
2. disposer d'une configuration préparée B complète et valide ;
3. acquisition arrêtée lorsque l'application l'exige ;
4. utiliser un `prepared_config_id` non nul distinct lorsque le scénario demande une nouvelle configuration ;
5. utiliser un `transaction_id` neuf pour chaque nouvelle commande ;
6. conserver un snapshot complet de l'image active 4E et de `active_config_crc` avant l'essai.

## 2. LIM03-G01 — Vecteur CRC normatif

### Objectif

Vérifier le calcul CRC-32/IEEE 802.3 et la sérialisation imposée par la V1.

### Procédure

1. charger exactement le vecteur normatif n°1 sur les offsets 16 à 99 ;
2. mettre tous les registres non explicitement listés à 0 ;
3. calculer indépendamment le CRC selon la V1 ;
4. comparer au résultat `0x5207CCFC` ;
5. écrire ce CRC dans `prepared_config_crc` ;
6. demander l'application avec toutes les autres préconditions satisfaites.

### Verdict

Le CRC calculé doit être exactement `0x5207CCFC`. Tout autre résultat constitue une anomalie normative/implémentation.

## 3. LIM03-G02 — Détection d'un CRC préparé incohérent

### Stratégie

Partir d'une configuration préparée valide B et tester séparément :
- modification d'un registre couvert sans mise à jour du CRC ;
- altération du CRC seul ;
- restauration d'un CRC correct après modification valide.

### Verdict invalide

Le firmware doit détecter l'incohérence lors de l'application, refuser l'activation et préserver intégralement l'image active A.

## 4. LIM03-G03 — Périmètre du CRC préparé

### Objectif

Vérifier que seuls les offsets 16..99 contribuent à `prepared_config_crc`.

### Stratégie

Comparer deux calculs :
- modification d'un registre couvert par 4B+4C+4D : le CRC attendu doit changer ;
- modification d'un champ hors périmètre, notamment `prepared_config_id` en 4A : le CRC attendu de 4B+4C+4D doit rester identique.

FT-LIM-03 ne prétend pas qu'un champ hors périmètre est sans effet fonctionnel global ; il vérifie uniquement son absence du calcul CRC préparé.

## 5. LIM03-G04 — Validation réussie et application

### Objectif

Vérifier la séquence logique d'une configuration préparée complète et valide.

### Verdict

- la configuration préparée est en `BROUILLON` après modification ;
- une validation réussie correspond logiquement à `VALIDE` ;
- une application réussie conduit à `ACTIF` ;
- 4E reflète de manière cohérente la configuration appliquée ;
- `active_config_crc` correspond au CRC calculé sur l'image 4E.

L'état `VALIDE` n'a pas de durée minimale d'observabilité imposée si l'implémentation enchaîne validation et application atomiquement.

## 6. LIM03-G05 — Validation échouée

### Stratégie

Introduire une configuration préparée dont une invalidité est déjà couverte et connue de FT-LIM-01 ou FT-LIM-02, avec CRC cohérent avec cette image invalide.

### Verdict

- l'écriture préparée reste acceptée au niveau Modbus ;
- lors de la tentative de validation/application, la configuration n'est pas activée ;
- `ERREUR_VALIDATION` doit être produit lorsque l'échec correspond à la phase de validation ;
- l'image active A et son CRC restent inchangés.

Le test peut utiliser comme oracle un cas invalide déjà normativement défini, sans dupliquer toute sa couverture de domaine.

## 7. LIM03-G06 — Configuration préparée incomplète

### Objectif

Vérifier qu'une configuration préparée incomplète ne peut être appliquée.

### Verdict

Lorsque la cause est sans ambiguïté une préparation incomplète, la commande Bloc 5 doit terminer sans activation et exposer `cmd_result_code = 20`.

Aucune définition supplémentaire de la notion de « complète » n'est inventée : le scénario doit utiliser un état dont l'incomplétude est explicitement démontrable par les exigences déjà normatives (par exemple identifiant requis à 0 ou champ obligatoire non configuré).

## 8. LIM03-G07 — Correction après ERREUR_VALIDATION

### Procédure

1. provoquer `ERREUR_VALIDATION` avec une configuration invalide ;
2. corriger au moins un champ préparé concerné ;
3. observer le retour à `BROUILLON` ;
4. recalculer le CRC ;
5. si la configuration devient complète et valide, demander l'application.

### Verdict

La modification préparée doit ramener l'état à `BROUILLON`. Une configuration corrigée peut ensuite être validée/appliquée normalement.

## 9. LIM03-G08 — Application interdite sans validation

### Objectif

Prouver l'absence de transition directe `VIDE -> ACTIF` et l'impossibilité d'activer une préparation non validable.

### Verdict

La commande d'application ne doit jamais produire `ACTIF` à partir d'un état préparé non validable. L'image active précédente, s'il en existe une, reste inchangée.

## 10. LIM03-G09 — Échec d'application depuis VALIDE

### Objectif

Vérifier la transition `VALIDE -> ERREUR_APPLICATION` quand la validation est acquise mais que l'application échoue pour une cause d'application distincte.

### Condition d'exécutabilité

Le banc doit pouvoir provoquer de manière maîtrisée une cause d'échec située après validation, sans rendre la configuration elle-même invalide.

Si aucun moyen normatif et contrôlable n'existe, l'instance est `N/A — APPLICATION_FAILURE_NOT_INJECTABLE`.

### Verdict

- état final `ERREUR_APPLICATION` ;
- nouvelle configuration non active ;
- image active précédente inchangée.

## 11. LIM03-G10 — Correction après ERREUR_APPLICATION

Après un `ERREUR_APPLICATION` réellement obtenu, toute modification d'au moins un champ préparé de 4B+4C+4D doit ramener la configuration à `BROUILLON`.

Si G09 n'est pas exécutable, G10 est également N/A.

## 12. LIM03-G11 — Nouvelle préparation depuis ACTIF

### Procédure

1. partir d'une configuration A en `ACTIF` ;
2. préparer une configuration B distincte en modifiant au moins un champ de 4B+4C+4D ;
3. observer l'état préparé ;
4. vérifier immédiatement l'image active.

### Verdict

- l'état de préparation revient à `BROUILLON` ;
- A reste l'image active tant que B n'est pas appliquée ;
- aucun effet immédiat de B n'est visible dans 4E.

## 13. LIM03-G12 — Cohérence et atomicité de l'image active

### Objectif

Vérifier qu'une application réussie met à jour l'image active comme une image cohérente et qu'un échec ne produit aucune mise à jour partielle.

### Verdict

Après succès : tous les champs actifs concernés correspondent à la même configuration B et `active_config_crc` correspond à 4E.

Après échec : tous les champs restent ceux de A et `active_config_crc` reste celui de A.

## 14. Critères d'acceptation

Une activation partielle, un CRC actif incohérent, une transition directe interdite vers ACTIF ou la perte de l'image active précédente en cas d'échec sont des FAIL majeurs.

`config_error_code` est enregistré comme observation seulement, sauf évolution normative ultérieure.
