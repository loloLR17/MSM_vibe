# Modbus RTU TR2 — Backlog normatif consolidé V1.1

## 1. Statut

Ce document consolide les évolutions candidates identifiées pendant l'audit transversal V1, l'architecture firmware Modbus RTU V1 et l'arbitrage K1 relatif à l'idempotence B5.

Il est **informatif et non normatif**.

La spécification Modbus RTU V1 reste l'unique oracle normatif tant qu'un candidat n'a pas été :
1. arbitré explicitement ;
2. intégré dans une spécification normative future ;
3. propagé dans les mappings et validations concernés ;
4. soumis à une passe de non-régression puis gelé.

La présence d'un sujet dans ce backlog ne constitue donc ni une exigence V1, ni une exigence V1.1 déjà décidée.

Baseline de consolidation :
- repository : `loloLR17/MSM_vibe` ;
- branche : `main` ;
- commit observé lors de la consolidation : `1805bda1f116de535ac0248c8086c259be553942` ;
- commit : `Architecture: align boot recovery with K1` ;
- date : 2026-09-01.

## 2. Règles de lecture

Les classifications suivantes doivent rester distinctes :

- `V1` : comportement explicitement défini par la baseline normative V1 ;
- `NOT_DEFINED V1` : absence d'oracle V1 suffisamment précis ; aucun comportement normatif ne doit être inventé ;
- `FW_POLICY` : choix firmware conservateur permettant d'implémenter V1 sans prétendre compléter la norme ;
- `IMPLEMENTATION` : choix technique interne sans portée protocolaire ;
- `V1.1 CANDIDATE` : sujet retenu pour analyse normative future, sans exigence encore décidée.

Un `NOT_DEFINED V1` ne devient pas automatiquement un `V1.1 CANDIDATE`.

Une bonne pratique, une préférence d'exploitation, un besoin de test ou un choix d'architecture ne deviennent pas automatiquement des exigences protocolaires.

## 3. Priorisation

- `V1.1-CRITICAL` : absence de règle susceptible de compromettre directement l'idempotence, le recovery sûr, l'interopérabilité fondamentale ou la représentation déterministe d'un état nécessaire à l'implémentation ;
- `IMPORTANT` : clarification normative ayant un impact réel d'interopérabilité ou d'interprétation, sans blocage immédiat comparable aux sujets critiques ;
- `OPTIONAL` : évolution utile mais non nécessaire pour fermer une ambiguïté fondamentale de V1 ;
- `DEFER` : sujet conservé pour traçabilité mais hors périmètre actif de clarification V1.1 ;
- `REJECT` : sujet explicitement écarté du backlog normatif.

Répartition après consolidation :
- `V1.1-CRITICAL` : 6 ;
- `IMPORTANT` : 21 ;
- `OPTIONAL` : 6 ;
- `DEFER` : 3 ;
- `REJECT` : 0.

## 4. Backlog consolidé

### V11-ID-01 — Unicité et cycle de vie de `device_id`

- **Domaine** : B0 — Identification
- **Origine** : audit transversal V1 ; registre des limites
- **Règle V1 actuelle** : `device_id` est exposé ; la politique industrielle de génération et de garantie d'unicité n'est pas définie.
- **Limite observée** : deux équipements pourraient être provisionnés avec une identité non suffisamment garantie sans violer un oracle V1 explicite sur le procédé industriel.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : risque de confusion d'identité capteur côté centrale et archivage.
- **Impact safety / idempotence** : indirect ; peut affecter la stabilité des identités composites utilisées hors B5.
- **Contournement V1 actuel** : politique industrielle de provisioning hors protocole.
- **Besoin réel V1.1** : clarifier au minimum la propriété d'identité attendue sans imposer nécessairement le mécanisme de provisioning.
- **Risque de compatibilité** : moyen si une future règle invalide des identités déjà déployées.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-ID-02 — Compatibilité HW / FW / protocole / centrale

- **Domaine** : B0 / transversal
- **Origine** : audit transversal V1 ; backlog historique
- **Règle V1 actuelle** : les informations de version existent selon la spécification, sans politique exhaustive de compatibilité inter-version.
- **Limite observée** : une centrale peut connaître des versions sans savoir si une combinaison est supportée.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : interprétation ou usage incorrect d'un capteur incompatible.
- **Impact safety / idempotence** : faible à indirect.
- **Contournement V1 actuel** : matrice de compatibilité gérée hors protocole.
- **Besoin réel V1.1** : déterminer si une règle minimale de compatibilité doit être standardisée.
- **Risque de compatibilité** : moyen.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-ID-03 — Immutabilité du numéro de série

- **Domaine** : B0 — Identification
- **Origine** : backlog historique
- **Règle V1 actuelle** : l'immutabilité industrielle du numéro de série n'est pas explicitement imposée.
- **Limite observée** : ambiguïté sur sa stabilité au cours de la vie du produit.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : traçabilité produit.
- **Impact safety / idempotence** : faible.
- **Contournement V1 actuel** : politique de fabrication et de maintenance.
- **Besoin réel V1.1** : uniquement si la stabilité du numéro de série doit devenir une propriété interopérable.
- **Risque de compatibilité** : faible à moyen.
- **Décision** : `OPTIONAL`
- **Proposition normative** :

### V11-ID-04 — Localisation / installation du capteur

- **Domaine** : B0 — Identification
- **Origine** : backlog historique ; registre des limites
- **Règle V1 actuelle** : aucune information normative de localisation/installation n'est imposée.
- **Limite observée** : la position fonctionnelle du capteur doit être gérée ailleurs.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : exploitabilité et association mesure/équipement.
- **Impact safety / idempotence** : nul.
- **Contournement V1 actuel** : gestion par la centrale, la campagne ou le système d'exploitation.
- **Besoin réel V1.1** : à justifier par un besoin système concret.
- **Risque de compatibilité** : faible.
- **Décision** : `OPTIONAL`
- **Proposition normative** :

### V11-SYS-01 — Cohérence `system_status`, health, flags et données B1/B7

- **Domaine** : B1 / B7
- **Origine** : audit transversal V1 ; registre des limites
- **Règle V1 actuelle** : plusieurs états et flags sont exposés, sans dérivation exhaustive ni relation complète B1 ↔ B7.
- **Limite observée** : deux projections individuellement légales peuvent devenir contradictoires.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : diagnostic ambigu côté centrale.
- **Impact safety / idempotence** : faible à indirect.
- **Contournement V1 actuel** : firmware cohérent par politique interne sans prétendre définir une bijection normative.
- **Besoin réel V1.1** : définir les invariants réellement nécessaires, sans surspécifier toutes les combinaisons.
- **Risque de compatibilité** : moyen à élevé.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-SYS-02 — Catalogues des codes diagnostic principaux

- **Domaine** : B1 / B7
- **Origine** : backlog historique ; registre des limites
- **Règle V1 actuelle** : les valeurs non nulles de plusieurs codes ne disposent pas toutes d'un catalogue exhaustif.
- **Limite observée** : une centrale générique ne peut pas interpréter tous les codes possibles de façon portable.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : diagnostic incomplet.
- **Impact safety / idempotence** : indirect.
- **Contournement V1 actuel** : documentation produit ou codes firmware privés.
- **Besoin réel V1.1** : définir les codes qui doivent être interopérables.
- **Risque de compatibilité** : moyen.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-SYS-03 — Priorité des erreurs, avertissements et défauts multiples

- **Domaine** : B1 / B7
- **Origine** : backlog historique ; registre des limites
- **Règle V1 actuelle** : la priorité entre causes simultanées n'est pas exhaustive.
- **Limite observée** : deux firmwares peuvent sélectionner des codes principaux différents pour les mêmes faits.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : diagnostic non déterministe.
- **Impact safety / idempotence** : faible.
- **Contournement V1 actuel** : priorité interne firmware.
- **Besoin réel V1.1** : définir une priorité seulement si un code principal unique doit être comparable entre implémentations.
- **Risque de compatibilité** : moyen.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-SYS-04 — Relation `acquisition_state` ↔ `active_campaign_id`

- **Domaine** : B1 / B6
- **Origine** : backlog historique ; registre des limites
- **Règle V1 actuelle** : les deux informations sont exposées sans relation exhaustive définie.
- **Limite observée** : états croisés potentiellement incohérents.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : centrale incapable de déterminer sans heuristique si une campagne est réellement active.
- **Impact safety / idempotence** : indirect sur les commandes de campagne.
- **Contournement V1 actuel** : cohérence assurée par l'architecture firmware.
- **Besoin réel V1.1** : fixer les invariants croisés nécessaires.
- **Risque de compatibilité** : moyen.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-DOM-01 — Domaines physiques et pourcentages

- **Domaine** : B1 / B7
- **Origine** : backlog historique ; registre des limites
- **Règle V1 actuelle** : certains seuils, domaines physiques ou bornes fonctionnelles ne sont pas exhaustivement normés.
- **Limite observée** : interprétation parfois dépendante de la définition produit.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : variable selon la grandeur.
- **Impact safety / idempotence** : nul à faible.
- **Contournement V1 actuel** : spécification produit / firmware.
- **Besoin réel V1.1** : faible tant qu'aucune ambiguïté protocolaire concrète n'est démontrée.
- **Risque de compatibilité** : moyen si des plages existantes sont resserrées.
- **Décision** : `DEFER`
- **Proposition normative** :

### V11-TIME-01 — Modèle complet de `time_status` et état DEGRADED

- **Domaine** : B2 — Temps
- **Origine** : audit transversal V1 ; registre des limites ; arbitrage K3
- **Règle V1 actuelle** : machine d'état exhaustive, critères complets de DEGRADED et priorités entre causes temporelles non définis.
- **Limite observée** : mêmes faits temporels susceptibles de produire des états différents.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : confiance temporelle interprétée différemment par la centrale.
- **Impact safety / idempotence** : indirect, notamment sur datation et synchronisation.
- **Contournement V1 actuel** : politique temporelle firmware conservatrice K3 ; `CONTINUITY_INDETERMINATE` n'est pas assimilé automatiquement à `DEGRADED` et `SYNCHRONIZED` exige une continuité prouvée.
- **Besoin réel V1.1** : définir les transitions, critères de DEGRADED et invariants réellement interopérables.
- **Risque de compatibilité** : moyen à élevé.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-TIME-02 — Validité temporelle après boot et `time_since_sync`

- **Domaine** : B2 / Boot-Recovery
- **Origine** : architecture firmware Modbus V1 ; arbitrage K3
- **Règle V1 actuelle** : les critères exacts permettant de considérer le temps civil valide après boot, de prouver la continuité d'une synchronisation historique et de représenter l'indisponibilité de `last_sync_time` / `time_since_sync_s` ne sont pas entièrement définis.
- **Limite observée** : un reboot peut casser la continuité des informations temporelles exposées ; V1 impose des champs `uint32` mais ne fournit pas de sentinel normative ni d'indicateur d'indisponibilité explicite pour ces deux informations.
- **Classification actuelle** : `NOT_DEFINED V1` sur la représentabilité / `FW_POLICY` K3 définie pour le confinement V1 / `V1.1 CANDIDATE`
- **Impact opérationnel** : interprétation de la fraîcheur de synchronisation, distinction entre heure civile utilisable et heure encore démontrablement synchronisée, et lecture des valeurs numériques en situation d'indisponibilité.
- **Impact safety / idempotence** : indirect ; important pour la fidélité temporelle des historiques et la non-fabrication de timestamps.
- **Contournement V1 actuel** : K3 sépare `civil_time_usable`, `sync_continuity_proven`, `LastSyncHistory` et `TimeSinceSync`; la continuité n'est reconnue que sur preuve positive ; `time_since_sync` reste `UNAVAILABLE` si la continuité n'est pas prouvée. Pour la projection V1, `time_since_sync_s = 0xFFFFFFFF` lorsque la durée est indisponible et, en cas de `NEVER_SYNCHRONIZED`, `last_sync_time = 0` avec `SYNC_PERFORMED = 0`; ces valeurs sont explicitement des conventions `FW_POLICY`, jamais des sentinelles normatives V1.
- **Besoin réel V1.1** : définir explicitement la représentation de l'indisponibilité de `last_sync_time` et `time_since_sync_s` — sentinelles réservées, indicateurs de validité/disponibilité ou mécanisme équivalent — ainsi que les invariants observables nécessaires après reboot, sans imposer le mécanisme matériel de preuve de continuité.
- **Risque de compatibilité** : moyen à élevé si une future représentation réserve des valeurs actuellement valides ou modifie l'interprétation de combinaisons status/flags existantes.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-CFG-01 — Catalogue et sémantique de `config_error_code`

- **Domaine** : B4 — Configuration
- **Origine** : registre des limites
- **Règle V1 actuelle** : catalogue exhaustif non défini.
- **Limite observée** : codes d'erreur non portables entre implémentations.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : diagnostic de configuration incomplet.
- **Impact safety / idempotence** : indirect.
- **Contournement V1 actuel** : documentation firmware privée.
- **Besoin réel V1.1** : définir les erreurs qui doivent être interprétables par une centrale générique.
- **Risque de compatibilité** : moyen.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-CFG-02 — Projection B4 sans configuration active récupérable

- **Domaine** : B4 / Boot-Recovery
- **Origine** : architecture firmware Modbus V1
- **Règle V1 actuelle** : aucune projection B4 exhaustive n'est définie pour le cas où aucune configuration active autoritative n'est récupérable et où le staging est vide.
- **Limite observée** : le firmware ne doit ni inventer une configuration active ni publier un état B4 ambigu.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : état de configuration impossible à interpréter de façon déterministe.
- **Impact safety / idempotence** : indirect mais important pour un démarrage sûr.
- **Contournement V1 actuel** : confinement firmware conservateur ; aucune configuration active fictive.
- **Besoin réel V1.1** : définir une représentation protocolaire explicite de cette absence.
- **Risque de compatibilité** : moyen.
- **Décision** : `V1.1-CRITICAL`
- **Proposition normative** :

### V1.1-TRANSACTION-01 — Cycle de vie normatif du `transaction_id`

- **Domaine** : B5 — Commandes / Idempotence
- **Origine** : backlog historique ; arbitrage K1
- **Règle V1 actuelle** : profondeur/durée de mémoire d'idempotence et politique générale de réutilisation/wrap non définies.
- **Limite observée** : aucune durée ou condition normative ne dit quand un identifiant ancien peut redevenir utilisable.
- **Classification actuelle** : `NOT_DEFINED V1` ; confinement firmware `lifetime strict` en `FW_POLICY`
- **Impact opérationnel** : durée de vie transactionnelle de la centrale non définie.
- **Impact safety / idempotence** : majeur ; une mauvaise réutilisation peut provoquer confusion ou double exécution.
- **Contournement V1 actuel** : tout txid admis reste définitivement connu par le firmware V1.
- **Besoin réel V1.1** : définir le lifecycle normatif, éventuellement via fenêtre, epoch, recyclage ou mécanisme équivalent.
- **Risque de compatibilité** : élevé.
- **Décision** : `V1.1-CRITICAL`
- **Proposition normative** :

### V1.1-TRANSACTION-02 — Épuisement du namespace `transaction_id`

- **Domaine** : B5 — Commandes / Idempotence
- **Origine** : arbitrage K1
- **Règle V1 actuelle** : aucune réponse protocolaire explicite lorsque les 65535 identifiants valides ont tous été admis sous une politique sans réutilisation.
- **Limite observée** : l'espace `1..65535` est fini et l'état d'épuisement devient inévitable sous `lifetime strict`.
- **Classification actuelle** : `NOT_DEFINED V1` ; confinement par `FW_POLICY`
- **Impact opérationnel** : impossibilité future d'admettre de nouvelles transactions.
- **Impact safety / idempotence** : majeur.
- **Contournement V1 actuel** : ne jamais libérer implicitement un txid déjà connu.
- **Besoin réel V1.1** : définir le comportement à l'épuisement et déterminer si son approche ou son atteinte doit être observable.
- **Risque de compatibilité** : élevé.
- **Décision** : `V1.1-CRITICAL`
- **Proposition normative** :

### V1.1-TRANSACTION-03 — Même txid avec requête différente

- **Domaine** : B5 — Commandes / Idempotence
- **Origine** : backlog historique ; arbitrage K1
- **Règle V1 actuelle** : réponse protocolaire exacte non définie.
- **Limite observée** : une collision doit être distinguée d'un retry cohérent.
- **Classification actuelle** : `NOT_DEFINED V1`; détection et non-redispatch en `FW_POLICY`
- **Impact opérationnel** : diagnostic d'une erreur de centrale ou collision d'identité.
- **Impact safety / idempotence** : majeur ; aucun redispatch métier ne doit être provoqué.
- **Contournement V1 actuel** : reconnaître la collision et interdire le redispatch.
- **Besoin réel V1.1** : définir la représentation B5 exacte.
- **Risque de compatibilité** : moyen à élevé.
- **Décision** : `V1.1-CRITICAL`
- **Proposition normative** :

### V1.1-TRANSACTION-04 — Transaction `RESERVED` interrompue avant effet

- **Domaine** : B5 / Boot-Recovery
- **Origine** : arbitrage K1
- **Règle V1 actuelle** : aucun mapping B5 exact d'une transaction durablement admise mais interrompue avant `STARTED`.
- **Limite observée** : l'absence d'effet est prouvable, mais aucun résultat V1 dédié ne représente ce cas.
- **Classification actuelle** : `NOT_DEFINED V1`; recovery sûr défini en `FW_POLICY`
- **Impact opérationnel** : résultat historique ambigu après reboot.
- **Impact safety / idempotence** : majeur ; la transaction reste protégée et ne doit pas être redispatchée.
- **Contournement V1 actuel** : finalisation interne durable d'interruption avant effet, sans inventer le mapping B5.
- **Besoin réel V1.1** : définir la représentation protocolaire.
- **Risque de compatibilité** : moyen.
- **Décision** : `V1.1-CRITICAL`
- **Proposition normative** :

### V1.1-TRANSACTION-05 — Transaction post-crash `INDETERMINATE`

- **Domaine** : B5 / Boot-Recovery
- **Origine** : arbitrage K1
- **Règle V1 actuelle** : aucun mapping B5 exact lorsqu'un effet terminal ou son absence ne peut être prouvé après crash.
- **Limite observée** : l'état métier courant ne suffit pas toujours à établir la causalité transactionnelle.
- **Classification actuelle** : `NOT_DEFINED V1`; protection contre replay en `FW_POLICY`
- **Impact opérationnel** : résultat historique non déterminable.
- **Impact safety / idempotence** : maximal ; rejouer pourrait dupliquer un effet déjà produit.
- **Contournement V1 actuel** : conserver le txid comme occupé et ne jamais redispatcher.
- **Besoin réel V1.1** : représenter explicitement l'indétermination sans suggérer un retry exécutable.
- **Risque de compatibilité** : élevé.
- **Décision** : `V1.1-CRITICAL`
- **Proposition normative** :

### V1.1-TRANSACTION-06 — `cmd_last_timestamp` sans heure civile valide

- **Domaine** : B5 / Temps
- **Origine** : arbitrage K1
- **Règle V1 actuelle** : représentation exacte d'un timestamp de terminaison indisponible non définie.
- **Limite observée** : un reboot ou retry ne doit pas fabriquer un nouvel instant historique.
- **Classification actuelle** : `NOT_DEFINED V1`; non-invention en `FW_POLICY`
- **Impact opérationnel** : ambiguïté sur la datation de la dernière transaction.
- **Impact safety / idempotence** : faible pour l'exécution, important pour la fidélité historique.
- **Contournement V1 actuel** : conserver l'absence de timestamp valide sans en inventer un.
- **Besoin réel V1.1** : définir un sentinel ou une représentation équivalente.
- **Risque de compatibilité** : moyen.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-CMD-01 — Refus concurrents et priorité des causes

- **Domaine** : B5 — Commandes
- **Origine** : backlog historique ; registre des limites
- **Règle V1 actuelle** : représentation exacte des champs actifs et priorité entre plusieurs causes de refus non exhaustives.
- **Limite observée** : même requête susceptible de produire des diagnostics différents.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : diagnostic de refus non déterministe.
- **Impact safety / idempotence** : faible à indirect.
- **Contournement V1 actuel** : une seule commande active ; politique de priorité interne.
- **Besoin réel V1.1** : définir ce qui doit être stable entre implémentations.
- **Risque de compatibilité** : moyen.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-CMD-02 — Annulation des commandes

- **Domaine** : B5 — Commandes
- **Origine** : backlog historique ; registre des limites
- **Règle V1 actuelle** : liste exhaustive des commandes annulables et cycle complet d'annulation non définis.
- **Limite observée** : `cancel_request` ne dispose pas d'un contrat fonctionnel complet.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : comportement d'annulation dépendant du firmware.
- **Impact safety / idempotence** : moyen selon la commande.
- **Contournement V1 actuel** : comportement limité à ce qui est explicitement défini.
- **Besoin réel V1.1** : définir le cycle seulement pour les commandes réellement annulables.
- **Risque de compatibilité** : moyen.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-CMD-03 — Effet de `clear_request_fields`

- **Domaine** : B5 — Commandes
- **Origine** : backlog historique ; registre des limites
- **Règle V1 actuelle** : effet exact non exhaustivement défini.
- **Limite observée** : implémentations susceptibles de nettoyer des ensembles de registres différents.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : état de mailbox différent après action.
- **Impact safety / idempotence** : faible.
- **Contournement V1 actuel** : politique firmware conservatrice.
- **Besoin réel V1.1** : définir précisément les champs affectés si la fonction est conservée.
- **Risque de compatibilité** : faible à moyen.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-CMD-04 — Catalogue `cmd_result_detail`

- **Domaine** : B5 — Commandes
- **Origine** : backlog historique ; registre des limites
- **Règle V1 actuelle** : sémantique exhaustive non définie.
- **Limite observée** : valeur potentiellement spécifique au firmware.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : diagnostic de commande non portable.
- **Impact safety / idempotence** : indirect.
- **Contournement V1 actuel** : utilisation minimale des valeurs normées et documentation privée éventuelle.
- **Besoin réel V1.1** : définir les détails qui doivent être interopérables.
- **Risque de compatibilité** : moyen.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-CMD-05 — SELFTEST : masque, résultat, effets et interruption

- **Domaine** : B5 / B7
- **Origine** : backlog historique ; architecture firmware
- **Règle V1 actuelle** : plusieurs aspects du SELFTEST restent incomplets : masque étendu éventuel, catalogues résultat/detail, effets observables et représentation d'un autotest interrompu.
- **Limite observée** : une centrale ne peut pas interpréter exhaustivement tous les résultats et cas de recovery.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : diagnostic et maintenance.
- **Impact safety / idempotence** : faible à moyen.
- **Contournement V1 actuel** : `selftest running` reste volatile ; seul un autotest complètement terminé peut être restauré ; un reboot ne crée pas automatiquement un échec.
- **Besoin réel V1.1** : définir le contrat SELFTEST observable sans imposer son implémentation interne.
- **Risque de compatibilité** : moyen.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-CMD-06 — Portée exacte de REFRESH

- **Domaine** : B5 — Commandes
- **Origine** : backlog historique ; registre des limites
- **Règle V1 actuelle** : portée au-delà des règles déjà normées non exhaustive.
- **Limite observée** : attentes possibles différentes sur ce qui est réellement rafraîchi.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : exploitabilité.
- **Impact safety / idempotence** : faible.
- **Contournement V1 actuel** : limiter l'effet à ce qui est explicitement défini.
- **Besoin réel V1.1** : seulement si un besoin d'exploitation concret exige une sémantique plus forte.
- **Risque de compatibilité** : faible.
- **Décision** : `OPTIONAL`
- **Proposition normative** :

### V11-CMD-07 — Mode maintenance

- **Domaine** : B5 — Commandes
- **Origine** : backlog historique
- **Règle V1 actuelle** : règles détaillées de mode maintenance et emploi exhaustif des codes associés non définis.
- **Limite observée** : comportement produit potentiellement spécifique.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : maintenance locale et distante.
- **Impact safety / idempotence** : faible à moyen selon les fonctions autorisées.
- **Contournement V1 actuel** : politique firmware limitée au contrat explicitement défini.
- **Besoin réel V1.1** : à justifier par le besoin produit.
- **Risque de compatibilité** : moyen.
- **Décision** : `OPTIONAL`
- **Proposition normative** :

### V11-CMD-08 — RESET SOFTWARE et opérations critiques

- **Domaine** : B5 / Boot-Recovery
- **Origine** : backlog historique ; architecture firmware
- **Règle V1 actuelle** : portée au-delà des règles normées et notion exhaustive d'opération critique non définies.
- **Limite observée** : interaction possible entre reset commandé et opération métier en cours.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : disponibilité et recovery.
- **Impact safety / idempotence** : moyen à élevé.
- **Contournement V1 actuel** : `BootIntent` durable et politiques firmware de protection des opérations.
- **Besoin réel V1.1** : clarifier les refus/préconditions qui doivent être identiques entre implémentations.
- **Risque de compatibilité** : moyen.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-CMD-09 — RESET STATISTICS

- **Domaine** : B5 / Diagnostic
- **Origine** : backlog historique ; architecture firmware
- **Règle V1 actuelle** : portée exacte et masque éventuel non définis au-delà des règles existantes.
- **Limite observée** : ensembles statistiques effacés potentiellement différents.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : comparabilité des statistiques.
- **Impact safety / idempotence** : faible.
- **Contournement V1 actuel** : statistiques de service séparées des historiques critiques.
- **Besoin réel V1.1** : uniquement si la commande reste fonctionnellement importante.
- **Risque de compatibilité** : moyen.
- **Décision** : `OPTIONAL`
- **Proposition normative** :

### V11-CAMP-01 — Réutilisation historique de `campaign_id`

- **Domaine** : B6 — Inventaire campagnes
- **Origine** : backlog historique ; registre des limites
- **Règle V1 actuelle** : réutilisation après suppression non définie.
- **Limite observée** : une identité historique peut entrer en collision avec une campagne ancienne connue de la centrale.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : ambiguïté d'archivage et de sélection de campagne.
- **Impact safety / idempotence** : indirect.
- **Contournement V1 actuel** : algorithme firmware crash-safe évitant les collisions parmi les campagnes valides présentes ; algorithme exact `IMPLEMENTATION`.
- **Besoin réel V1.1** : définir la règle de réutilisation historique, pas l'algorithme d'allocation.
- **Risque de compatibilité** : élevé pour les historiques existants.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-CAMP-02 — `duration_s` active et discontinuités temporelles

- **Domaine** : B6 / Temps
- **Origine** : backlog historique ; registre des limites
- **Règle V1 actuelle** : règle exacte pour campagne en cours et en présence d'une discontinuité temporelle non exhaustive.
- **Limite observée** : même campagne susceptible d'afficher des durées différentes selon firmware.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : interprétation de durée et corrélation temporelle.
- **Impact safety / idempotence** : faible.
- **Contournement V1 actuel** : politique firmware documentée hors norme.
- **Besoin réel V1.1** : définir une sémantique portable.
- **Risque de compatibilité** : moyen.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-STOR-01 — Relations used/free/capacity et compteurs

- **Domaine** : B6 — Stockage
- **Origine** : backlog historique ; registre des limites
- **Règle V1 actuelle** : relations exhaustives de capacité et certains invariants de compteurs ne sont pas imposés.
- **Limite observée** : métriques pouvant être calculées avec conventions différentes.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : supervision de capacité.
- **Impact safety / idempotence** : nul.
- **Contournement V1 actuel** : interprétation suivant documentation produit.
- **Besoin réel V1.1** : à justifier par un besoin d'exploitation.
- **Risque de compatibilité** : faible à moyen.
- **Décision** : `OPTIONAL`
- **Proposition normative** :

### V11-STOR-02 — `storage_health_status` et `data_integrity_status`

- **Domaine** : B6 — Stockage
- **Origine** : backlog historique ; audit final ; registre des limites
- **Règle V1 actuelle** : critères exhaustifs de dérivation non définis.
- **Limite observée** : mêmes défauts de stockage susceptibles de produire des statuts différents.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : confiance dans les données et le média.
- **Impact safety / idempotence** : indirect sur la disponibilité et le recovery.
- **Contournement V1 actuel** : critères firmware conservateurs.
- **Besoin réel V1.1** : définir les états qui doivent avoir une signification portable.
- **Risque de compatibilité** : moyen.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-CAMP-03 — Valeurs quand `selected_campaign_valid = 0`

- **Domaine** : B6 — Inventaire campagnes
- **Origine** : backlog historique ; registre des limites
- **Règle V1 actuelle** : les métadonnées doivent être ignorées lorsque la sélection est invalide, mais leurs valeurs exactes ne sont pas imposées.
- **Limite observée** : une centrale mal conçue peut surinterpréter des valeurs résiduelles.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : robustesse d'interprétation.
- **Impact safety / idempotence** : nul.
- **Contournement V1 actuel** : la centrale respecte `selected_campaign_valid` et ignore les autres métadonnées.
- **Besoin réel V1.1** : déterminer si un sentinel uniforme apporte une valeur suffisante.
- **Risque de compatibilité** : faible.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-CAMP-04 — Ordre stable de l'inventaire

- **Domaine** : B6 — Inventaire campagnes
- **Origine** : backlog historique ; registre des limites
- **Règle V1 actuelle** : ordre historique lors de l'évolution de l'inventaire non défini.
- **Limite observée** : index/listing potentiellement non stable.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : navigation centrale.
- **Impact safety / idempotence** : nul.
- **Contournement V1 actuel** : identifier les campagnes par `campaign_id`, pas par position historique supposée.
- **Besoin réel V1.1** : faible si l'identité de campagne est correctement utilisée.
- **Risque de compatibilité** : moyen si un ordre est imposé rétroactivement.
- **Décision** : `DEFER`
- **Proposition normative** :

### V11-DIAG-01 — `last_fault_timestamp` sans défaut connu

- **Domaine** : B7 — Diagnostic
- **Origine** : backlog historique ; registre des limites
- **Règle V1 actuelle** : valeur obligatoire lorsque `last_fault_code = 0` non définie.
- **Limite observée** : timestamp sans événement associé potentiellement ambigu.
- **Classification actuelle** : `NOT_DEFINED V1` / `V1.1 CANDIDATE`
- **Impact opérationnel** : interprétation historique.
- **Impact safety / idempotence** : nul.
- **Contournement V1 actuel** : ne pas inventer de timestamp si aucun défaut historique valide n'existe.
- **Besoin réel V1.1** : définir une représentation cohérente de l'absence.
- **Risque de compatibilité** : faible à moyen.
- **Décision** : `IMPORTANT`
- **Proposition normative** :

### V11-DIAG-02 — Historique diagnostic enrichi

- **Domaine** : Diagnostic / transversal
- **Origine** : audit final ; architecture firmware
- **Règle V1 actuelle** : V1 n'impose pas de journal circulaire de défauts ni d'historique enrichi.
- **Limite observée** : observabilité historique limitée au contrat actuel.
- **Classification actuelle** : extension fonctionnelle potentielle / `V1.1 CANDIDATE`
- **Impact opérationnel** : diagnostic terrain amélioré.
- **Impact safety / idempotence** : faible.
- **Contournement V1 actuel** : historique minimal existant et outils hors protocole.
- **Besoin réel V1.1** : non démontré pour fermer une ambiguïté V1.
- **Risque de compatibilité** : faible si ajout extensif, plus élevé si mapping existant modifié.
- **Décision** : `DEFER`
- **Proposition normative** :

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

Ils restent des limites V1 explicites. Une promotion future exigera un besoin normatif démontré.

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

Tant que ces étapes ne sont pas terminées, le champ **Proposition normative** d'un candidat reste vide et le contenu de ce document ne constitue pas un oracle.

## 8. Règle de poursuite de l'architecture firmware

Le backlog V1.1 reste ouvert pendant la poursuite de l'architecture firmware.

L'existence d'un candidat V1.1 n'autorise pas le firmware V1 à inventer le comportement normatif manquant.

Lorsque l'architecture V1 rencontre un `NOT_DEFINED V1`, elle doit :
- respecter strictement les exigences V1 existantes ;
- choisir si nécessaire une `FW_POLICY` conservatrice explicitement identifiée ;
- empêcher qu'une politique interne soit présentée comme une exigence protocolaire ;
- conserver la traçabilité vers le candidat V1.1 correspondant lorsque le besoin est réel.
