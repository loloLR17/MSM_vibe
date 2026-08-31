# FT-CMD-06 — Validation détaillée

## START ACQUISITION

### TT-CMD-B05-500 — Démarrage nominal
Préconditions : configuration active valide, SD exploitable, mémoire suffisante, aucun défaut critique bloquant, acquisition inactive.
Oracle FT-CMD : la commande 3 est acceptée et termine avec succès. L'effet `acquisition_state = en cours` est vérifié en FT-INT.

### TT-CMD-B05-501 — Aucune configuration active valide
Précondition : acquisition inactive, aucune configuration active valide, autres conditions rendues compatibles.
Oracle : refus avec `cmd_result_code = 22`.

### TT-CMD-B05-502 — SD absente
Précondition : config active valide, acquisition inactive, SD absente, autres conditions compatibles.
Oracle : refus avec `cmd_result_code = 6`.

### TT-CMD-B05-503 — Mémoire insuffisante
Précondition : config active valide, acquisition inactive, stockage présent mais capacité insuffisante selon l'état exposé par le DUT.
Oracle : refus avec `cmd_result_code = 7`.

### TT-CMD-B05-504 — Défaut critique actif
Précondition : config active valide, acquisition inactive, stockage compatible, défaut critique bloquant actif.
Oracle : refus avec `cmd_result_code = 8`.

### TT-CMD-B05-505 — Acquisition déjà active / état incompatible
Précondition : acquisition déjà active et absence des autres causes de refus ciblées.
Oracle : refus avec `cmd_result_code = 3`.

### TT-CMD-B05-506 — Causes simultanées
Classification : `TRACE_ONLY`.
La V1 ne fixe pas de priorité entre plusieurs causes simultanées de refus START. Ne pas exiger un code déterminé si plusieurs préconditions sont simultanément violées.

## STOP ACQUISITION

### TT-CMD-B05-507 — Arrêt nominal
Précondition : acquisition active.
Oracle FT-CMD : commande 4 acceptée et terminée avec succès. Les effets de fermeture de campagne restent FT-INT.

### TT-CMD-B05-508 — Acquisition non active
Précondition : acquisition arrêtée.
Oracle : refus avec `cmd_result_code = 21`.

## SELFTEST

### TT-CMD-B05-509 — Autotest standard nominal
Précondition : contexte compatible, `param1 = 0`.
Oracle FT-CMD : commande 5 prise en compte et résultat transactionnel cohérent. La publication de `selftest_status/result/detail` en Bloc 7 est FT-INT.

### TT-CMD-B05-510 — Échec autotest
Condition : banc capable de provoquer de manière contrôlée un échec d'autotest.
Oracle : résultat `11` (`échec autotest`).
Classification : `CONDITIONAL` si cette faute n'est pas injectable de façon déterministe.

### TT-CMD-B05-511 — Timeout interne autotest
Condition : mécanisme de test capable de provoquer le timeout interne normé.
Oracle : résultat `10`.
Classification : `CONDITIONAL`.

Le masque de sous-tests non nul n'est pas exigé : son support est explicitement conditionnel à l'implémentation de l'extension.

## ACK défaut / alarme

### TT-CMD-B05-512 — Acquittement nominal
Précondition : défaut présent et acquittable ; paramètres valides.
Oracle FT-CMD : commande 6 acceptée et terminée avec succès. La persistance éventuelle de la cause reste vérifiée en FT-INT.

### TT-CMD-B05-513 — Défaut non acquittable
Précondition : défaut présent identifié comme non acquittable.
Oracle : refus avec `cmd_result_code = 16`.

### TT-CMD-B05-514 — Paramètre ACK invalide
Précondition : construire une valeur de paramètre explicitement invalide selon l'implémentation normative disponible, sans extrapoler de domaine non défini.
Oracle : `cmd_result_code = 2`.
Classification : `CONDITIONAL` si aucun cas invalide concret n'est normativement constructible.

## REFRESH indicateurs

### TT-CMD-B05-515 — Rafraîchissement nominal
Précondition : contexte compatible.
Oracle FT-CMD : commande 7 prise en compte et terminée avec succès.
Le périmètre exact des indicateurs recalculés n'est pas défini dans le Bloc 5 ; l'absence de modification de configuration appartient à FT-INT.

## Règle générale d'essai

Pour tous les refus, isoler autant que possible une seule cause. Si plusieurs causes sont simultanément présentes et que la V1 ne définit pas de priorité, le test ne doit pas transformer l'ordre observé en exigence normative.
