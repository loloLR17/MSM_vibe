# Modbus RTU TR2 — Backlog normatif consolidé V1.1

## 1. Statut

Ce document consolide et requalifie les évolutions candidates identifiées pendant l'audit transversal V1, l'architecture firmware Modbus RTU V1 et les arbitrages V1.1 déjà gelés.

Il est **informatif et non normatif**.

La spécification Modbus RTU V1 reste l'oracle normatif V1. Les éléments V1.1 marqués `RESOLVED / FROZEN` renvoient à des gels V1.1 dédiés ; ce backlog ne remplace jamais leurs spécifications, mappings, validations ou documents de gel.

Baseline de requalification :
- repository : `loloLR17/MSM_vibe` ;
- branche : `main` ;
- commit observé avant cette requalification : `9f25cab8b29f5137bb1da78217bbeffd2584a5cb` ;
- commit : `Architecture: freeze V1.1 last timestamp validity` ;
- date : 2026-09-03.

## 2. Règles de lecture

Les classifications suivantes restent distinctes :

- `V1` : comportement explicitement défini par la baseline normative V1 ;
- `NOT_DEFINED V1` : absence d'oracle V1 suffisamment précis ; aucun comportement normatif ne doit être inventé ;
- `FW_POLICY` : choix firmware conservateur permettant d'implémenter V1 sans prétendre compléter la norme ;
- `IMPLEMENTATION` : choix technique interne sans portée protocolaire ;
- `V1.1 CANDIDATE` : sujet retenu pour analyse normative future ;
- `RESOLVED / FROZEN` : sujet du backlog dont le besoin normatif considéré a été arbitré, propagé et gelé dans les documents propriétaires ;
- `PARTIALLY RESOLVED` : cœur du sujet déjà fermé mais résiduel normatif encore identifié ;
- `OPEN / IMPORTANT` : clarification normative encore réellement utile à l'interopérabilité V1.1 ;
- `OPTIONAL` : évolution utile mais non nécessaire pour fermer une ambiguïté fondamentale ;
- `DEFER` : sujet conservé pour traçabilité mais hors périmètre actif ;
- `REJECT` : sujet explicitement écarté.

Un `NOT_DEFINED V1` ne devient pas automatiquement un `V1.1 CANDIDATE`. Une bonne pratique, une préférence d'exploitation, un besoin de test ou un choix d'architecture ne deviennent pas automatiquement des exigences protocolaires.

## 3. Synthèse de la requalification

Répartition des 36 sujets consolidés :

- `RESOLVED / FROZEN` : 6 ;
- `PARTIALLY RESOLVED` : 5 ;
- `OPEN / IMPORTANT` : 15 ;
- `OPTIONAL` : 6 ;
- `DEFER` : 4 ;
- `REJECT` : 0.

**Aucun `V1.1-CRITICAL` ne reste ouvert.**

La poursuite de l'implémentation firmware V1 n'est donc bloquée par aucun candidat de ce backlog. Les sujets ouverts concernent désormais principalement l'interopérabilité et la clarification V1.1. Les `FW_POLICY` gelées restent des politiques firmware et ne deviennent pas, par cette requalification, des exigences V1.

## 4. Backlog requalifié

### 4.1 Identification B0

| ID | Sujet | Statut | Résiduel / justification |
|---|---|---|---|
| V11-ID-01 | Unicité et cycle de vie de `device_id` | `PARTIALLY RESOLVED` | Architecture : `device_id` stable, unique, persistant, jamais régénéré silencieusement. Restent provisioning initial, reprovisioning et garantie industrielle d'unicité. |
| V11-ID-02 | Compatibilité HW / FW / protocole / centrale | `OPEN / IMPORTANT` | Politique minimale de compatibilité inter-version encore à définir si elle doit être portable. |
| V11-ID-03 | Immutabilité du numéro de série | `OPTIONAL` | À promouvoir seulement si cette stabilité doit devenir une propriété interopérable. |
| V11-ID-04 | Localisation / installation du capteur | `DEFER` | Gestion par la centrale ou le système d'installation suffisante tant qu'aucun besoin protocolaire concret n'est démontré. |

### 4.2 États système et diagnostic transversal B1/B7

| ID | Sujet | Statut | Résiduel / justification |
|---|---|---|---|
| V11-SYS-01 | Cohérence `system_status`, health, flags et B1/B7 | `OPEN / IMPORTANT` | `SystemStateAggregator` est architecturalement dérivé, mais les invariants protocolaires croisés restent à définir. |
| V11-SYS-02 | Catalogues des codes diagnostic principaux | `OPEN / IMPORTANT` | Définir uniquement les codes devant être interprétables par une centrale générique. |
| V11-SYS-03 | Priorité erreurs / avertissements / défauts multiples | `OPEN / IMPORTANT` | Priorité interopérable encore ouverte lorsqu'un code principal unique doit être exposé. |
| V11-SYS-04 | Relation `acquisition_state` ↔ `active_campaign_id` | `PARTIALLY RESOLVED` | Autorités Acquisition/Campaign séparées et recovery maîtrisé ; relation observable B1↔B6 encore ouverte. |
| V11-DOM-01 | Domaines physiques et pourcentages | `DEFER` | Aucun besoin normatif transversal concret démontré. |

### 4.3 Temps B2

| ID | Sujet | Statut | Résiduel / justification |
|---|---|---|---|
| V11-TIME-01 | Modèle complet de `time_status` et `DEGRADED` | `OPEN / IMPORTANT` | K3 confine le firmware ; machine normative exhaustive et critères de `DEGRADED` restent ouverts. |
| V11-TIME-02 | Validité après boot, `last_sync_time`, `time_since_sync_s` | `OPEN / IMPORTANT` | K3 définit la preuve de continuité et l'indisponibilité interne ; représentation protocolaire V1.1 de l'indisponibilité reste à arbitrer. |

### 4.4 Configuration B4

| ID | Sujet | Statut | Résiduel / justification |
|---|---|---|---|
| V11-CFG-01 | Catalogue et sémantique de `config_error_code` | `OPEN / IMPORTANT` | Définir seulement les causes qui doivent être interopérables. |
| V11-CFG-02 | Projection B4 sans configuration active récupérable | `OPEN / IMPORTANT` | K2 fournit une `FW_POLICY` déterministe et sûre ; représentation normative V1.1 reste ouverte mais n'est plus bloquante. |

### 4.5 Transactions B5

| ID | Sujet | Statut | Résiduel / justification |
|---|---|---|---|
| V1.1-TRANSACTION-01 | Cycle de vie du `transaction_id` | `RESOLVED / FROZEN` | `TransactionIdentity=(transaction_epoch, transaction_id)`, epoch persistante et renewal explicite gelés. |
| V1.1-TRANSACTION-02 | Épuisement du namespace `transaction_id` | `PARTIALLY RESOLVED` | Le renouvellement d'epoch ferme le problème principal. Résiduels : observabilité de saturation, renewal temporairement refusé, politique à `transaction_epoch=0xFFFFFFFF`. |
| V1.1-TRANSACTION-03 | Même identité avec requête différente | `RESOLVED / FROZEN` | Collision explicite, résultat 27, aucun redispatch. |
| V1.1-TRANSACTION-04 | `RESERVED` interrompu avant effet | `RESOLVED / FROZEN` | Résultat 28 `TRANSACTION_ABORTED_BEFORE_EFFECT`, identité consommée, aucun redispatch. |
| V1.1-TRANSACTION-05 [legacy backlog] | Transaction post-crash `INDETERMINATE` | `RESOLVED / FROZEN` | Représentation par `cmd_status=9 RECOVERY_INDETERMINATE`. Ce label historique ne doit pas être confondu avec l'arbitrage ultérieur TRANSACTION-05 `STARTED + ABSENCE_PROVEN`, gelé avec résultat 29. |
| V1.1-TRANSACTION-06 | `cmd_last_timestamp` indisponible | `RESOLVED / FROZEN` | `cmd_engine_flags.bit11=LAST_TIMESTAMP_VALID`, invariants T06 et fault injection J-T06 gelés ; aucun timestamp historique fabriqué. |

### 4.6 Commandes B5

| ID | Sujet | Statut | Résiduel / justification |
|---|---|---|---|
| V11-CMD-01 | Refus concurrents et priorité des causes | `PARTIALLY RESOLVED` | Ordre d'admission transactionnel V1.1 gelé ; priorité exhaustive des causes métier reste ouverte. |
| V11-CMD-02 | Annulation des commandes | `OPEN / IMPORTANT` | Liste des commandes annulables et cycle observable complet encore à définir. |
| V11-CMD-03 | Effet de `clear_request_fields` | `RESOLVED / FROZEN` | Mailbox V1.1 neutralisée selon le contrat gelé sans modifier transactions capturées, autorités persistantes, active ou last. |
| V11-CMD-04 | Catalogue `cmd_result_detail` | `OPEN / IMPORTANT` | Catalogue interopérable encore à définir. |
| V11-CMD-05 | SELFTEST : masque, résultat, effets, interruption | `OPEN / IMPORTANT` | Architecture recovery disponible ; contrat protocolaire observable exhaustif encore ouvert. |
| V11-CMD-06 | Portée exacte de REFRESH | `OPTIONAL` | À promouvoir seulement sur besoin d'exploitation démontré. |
| V11-CMD-07 | Mode maintenance | `OPTIONAL` | À promouvoir seulement sur besoin produit démontré. |
| V11-CMD-08 | RESET SOFTWARE et opérations critiques | `OPEN / IMPORTANT` | `BootIntent` et recovery sécurisent l'architecture ; préconditions/refus interopérables restent à clarifier. |
| V11-CMD-09 | RESET STATISTICS | `OPTIONAL` | Portée exacte à normaliser seulement si nécessaire. |

### 4.7 Campagnes et stockage B6

| ID | Sujet | Statut | Résiduel / justification |
|---|---|---|---|
| V11-CAMP-01 | Réutilisation historique de `campaign_id` | `PARTIALLY RESOLVED` | Unicité crash-safe garantie parmi les campagnes valides présentes ; réutilisation après suppression reste ouverte. |
| V11-CAMP-02 | `duration_s` active et discontinuités temporelles | `OPEN / IMPORTANT` | Recovery interdit déjà d'inventer durée/fin ; sémantique portable de `duration_s` reste ouverte. |
| V11-STOR-01 | Relations used/free/capacity et compteurs | `OPTIONAL` | Besoin d'exploitation à démontrer. |
| V11-STOR-02 | `storage_health_status` et `data_integrity_status` | `OPEN / IMPORTANT` | États devant avoir une signification portable encore à définir. |
| V11-CAMP-03 | Valeurs quand `selected_campaign_valid=0` | `OPTIONAL` | La centrale doit déjà ignorer les métadonnées lorsque la sélection est invalide ; uniformiser leurs valeurs n'est pas fondamental. |
| V11-CAMP-04 | Ordre stable de l'inventaire | `DEFER` | Utiliser `campaign_id`, pas une position historique supposée. |

### 4.8 Diagnostic B7

| ID | Sujet | Statut | Résiduel / justification |
|---|---|---|---|
| V11-DIAG-01 | `last_fault_timestamp` sans défaut connu | `OPEN / IMPORTANT` | L'architecture interdit la fabrication de timestamp ; représentation protocolaire explicite de l'absence reste ouverte. |
| V11-DIAG-02 | Historique diagnostic enrichi | `DEFER` | Extension fonctionnelle sans besoin normatif démontré pour fermer V1. |

## 5. Éléments explicitement hors backlog normatif

Les sujets suivants restent légitimes mais ne sont pas des exigences Modbus V1.1 en tant que telles :

- terminologie documentaire des frontières de persistance ;
- harmonisation documentaire des classifications propriété vs test ;
- fermeture des dettes historiques dans les README et fichiers de version ;
- spécification détaillée du banc matériel, des préconditions et des moyens d'essai ;
- observables de test ou mécanismes de fault injection nécessaires à la preuve d'une propriété ;
- algorithme concret de provisioning de `device_id` ;
- source physique du numéro de série ou des données constructeur ;
- algorithme exact d'allocation de `campaign_id` ;
- format physique du `CommandJournal`, fingerprint, index et technologie NVM ;
- format des chunks, filesystem, garbage collection et wear management ;
- RTOS, DMA, mutex, double buffering et primitives de synchronisation ;
- priorité matérielle des flags de reset lorsqu'elle n'est pas normée ;
- seuils purement techniques de protection interne non destinés à l'interopérabilité ;
- crypto/authentification tant qu'aucun périmètre protocolaire futur ne les introduit explicitement.

Ces sujets doivent être gérés dans les documents d'architecture, d'implémentation, de validation ou de produit appropriés.

## 6. Points non promus à ce stade

Le registre V1 contient d'autres `NOT_DEFINED` qui ne sont pas promus automatiquement dans ce backlog, notamment plusieurs limites B3 et certains détails B4.

Exemples :
- axe dominant B3 lorsque l'oracle n'est pas fourni ;
- dérivation exhaustive statut/validité/sévérité B3 ;
- formule complète fenêtre/échantillons valides B3 ;
- modèle transversal actif/mémorisé/acquitté B3 ;
- événement exact d'incrément de `config_revision_counter`.

Ils restent des limites V1 explicites. Une promotion future exige un besoin normatif démontré.

## 7. Processus de promotion

Pour promouvoir un candidat en exigence V1.1 :

1. établir le besoin normatif concret ;
2. arbitrer le comportement sans hypothèse implicite ;
3. identifier la spécification source propriétaire ;
4. rédiger l'exigence normative ;
5. analyser le risque de compatibilité V1 → V1.1 ;
6. vérifier charte de typage, registres et mapping ;
7. propager vers la famille de validation propriétaire ;
8. mettre à jour les dépendances transversales ;
9. ajouter ou modifier l'oracle de test ;
10. effectuer une passe de non-régression ;
11. geler explicitement la modification.

Un sujet n'est `RESOLVED / FROZEN` que lorsque son gel propriétaire établit ces éléments. Le backlog ne constitue jamais à lui seul l'oracle de la décision.

## 8. Règle de poursuite de l'architecture et de l'implémentation firmware

Aucun `V1.1-CRITICAL` n'est ouvert après cette requalification.

La poursuite de l'implémentation firmware V1 n'est bloquée par aucun candidat du backlog. Les sujets `OPEN / IMPORTANT` restent des travaux d'interopérabilité et de clarification V1.1.

L'existence d'un candidat V1.1 n'autorise jamais le firmware V1 à inventer le comportement normatif manquant. Lorsqu'un `NOT_DEFINED V1` est rencontré, le firmware doit :
- respecter strictement les exigences V1 existantes ;
- choisir si nécessaire une `FW_POLICY` conservatrice explicitement identifiée ;
- empêcher qu'une politique interne soit présentée comme une exigence protocolaire ;
- conserver la traçabilité vers le candidat V1.1 correspondant lorsque le besoin est réel.

Toute promotion future continue de suivre :

```text
arbitrage
→ spécification
→ mapping
→ validation
→ non-régression
→ gel
```
