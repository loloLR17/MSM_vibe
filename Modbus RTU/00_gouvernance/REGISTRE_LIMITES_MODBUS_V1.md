# Modbus RTU TR2 V1 — Registre consolidé des limites

## Objet

Ce document consolide les propriétés V1 restant `NOT_DEFINED`, `CONDITIONAL`, `TRACE_ONLY` ou `DELEGATED`. Il ne crée aucune exigence nouvelle : la spécification V1 reste l'oracle.

## Règles de lecture

- `NOT_DEFINED` : absence d'oracle V1 suffisamment précis ; aucun comportement ne doit être inventé.
- `CONDITIONAL` : oracle disponible mais essai dépendant d'une précondition ou d'un moyen d'essai déterministe.
- `TRACE_ONLY` : observation enregistrée sans verdict autonome lorsque la V1 n'impose pas de relation stricte.
- `DELEGATED` : propriété possédant un autre owner de validation.

## NOT_DEFINED significatifs

### B0
- politique industrielle de génération/unicité de `device_id` ;
- compatibilité détaillée entre versions ;
- localisation/installation du capteur.

### B1
- dérivation exhaustive `system_status` ↔ flags ;
- catalogues `error_code` / `warning_code` et priorités ;
- relation exhaustive B1 ↔ B7 ;
- relation exhaustive `acquisition_state` ↔ `active_campaign_id` ;
- seuils physiques non explicitement spécifiés.

### B2
- machine d'état exhaustive de `time_status` ;
- critères complets d'entrée/sortie de l'état dégradé ;
- priorités non explicitement spécifiées entre causes temporelles.

### B3
- axe dominant lorsque l'oracle n'est pas fourni ;
- dérivation exhaustive statut/validité/sévérité ;
- formule complète fenêtre/échantillons valides dans tous les cas ;
- modèle transversal actif/mémorisé/acquitté.

### B4
- événement exact d'incrément de `config_revision_counter` lorsqu'il n'est pas explicitement imposé ;
- catalogue exhaustif de `config_error_code`.

### B5
- profondeur/durée de mémoire d'idempotence ;
- même `transaction_id` avec payload différent ;
- réutilisation/bouclage des IDs au regard de l'historique ;
- représentation exacte de `cmd_active_*` lors d'un refus concurrent ;
- cycle complet d'annulation ;
- effet exact de `clear_request_fields` ;
- sémantique exhaustive de `cmd_result_detail` ;
- priorité entre causes simultanées de refus lorsqu'aucune n'est définie ;
- portée exacte de REFRESH, RESET SOFTWARE et RESET STATISTICS au-delà des règles déjà normées.

### B6
- réutilisation historique de `campaign_id` après suppression ;
- relation exacte de `duration_s` pour campagne en cours ;
- relation exacte de `duration_s` si une discontinuité temporelle affecte l'intervalle ;
- invariant `valid_campaign_count <= total_campaign_count` s'il devait devenir normatif ;
- relations exhaustives stockage utilisé/libre/capacité ;
- critères de `storage_health_status` et de `data_integrity_status` ;
- valeurs des métadonnées quand `selected_campaign_valid = 0` (elles doivent simplement être ignorées) ;
- ordre historique de la liste lorsque l'inventaire évolue.

### B7
- catalogue non nul de `last_fault_code` ;
- catalogues `selftest_result_code` / `selftest_detail` ;
- seuils fonctionnels non explicitement spécifiés ;
- dérivation exhaustive santé ↔ flags ;
- priorité entre défauts multiples ;
- égalités/tolérances B1 ↔ B7 non spécifiées ;
- valeur obligatoire de `last_fault_timestamp` lorsque `last_fault_code = 0`.

## CONDITIONAL significatifs

- états nécessitant une condition matérielle contrôlée pour être provoqués ;
- calculs vibratoires nécessitant des signaux connus et reproductibles ;
- contrôles de saturation ou de conservation nécessitant une précondition spécifique ;
- observation de `submit = 1` avant auto-clear si cet état est trop bref ;
- preuve de non-double-exécution nécessitant un effet observable distinctif ;
- commandes dont le refus ou l'échec exige un état matériel particulier ;
- SELFTEST échec/timeout nécessitant un moyen d'essai dédié ;
- campagnes nécessitant plusieurs entrées valides ou une campagne terminée sans discontinuité temporelle ;
- essais de persistance/reboot nécessitant un reset ou une coupure contrôlée.

## TRACE_ONLY significatifs

- proximité des uptimes B1/B7 sans tolérance normative ;
- comparaison des causes de reset B1/B7 sans relation exhaustive ;
- comparaison des températures B1/B7 sans tolérance normative ;
- observations croisées B1/B7 sans bijection définie ;
- champs de commande exposés sans cycle fonctionnel complet spécifié ;
- contrôles anti-surinterprétation servant à vérifier qu'aucun oracle privé n'est utilisé.

## Règle de gel

Ces limites sont compatibles avec le gel V1 : elles sont explicites et ne sont pas nécessaires pour implémenter le contrat protocolaire effectivement défini ni pour prononcer un verdict sur une propriété prétendue définie.

Toute réduction future de ce registre nécessite une modification normative explicite puis une analyse d'impact de validation.