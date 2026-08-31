# FT-PER-04 — Source normative consolidée

## PER04-R01 — Idempotence nominale sans reboot

**Classification : `DELEGATED` vers FT-CMD-02.**

La V1 impose qu'un `transaction_id` déjà traité ne provoque pas une seconde exécution et que le résultat précédent soit réutilisé. Cette propriété nominale reste hors FT-PER.

## PER04-R02 — Persistance de la mémoire d'idempotence après reboot

**Classification : `NOT_DEFINED`.**

La V1 ne précise ni profondeur ni durée de rétention de la mémoire d'idempotence, et ne dit pas si cette mémoire survit à un reboot.

## PER04-R03 — Rejeu post-reboot d'un transaction_id traité avant reboot

**Classification : `NOT_DEFINED`.**

Aucun oracle V1 ne permet d'imposer soit la réutilisation du résultat précédent, soit le traitement comme nouvelle transaction après reboot.

## PER04-R04 — Même transaction_id avec payload différent après reboot

**Classification : `NOT_DEFINED`.**

Même hors reboot, la priorité normative entre idempotence et incohérence de contenu n'est pas détaillée. Le reboot n'ajoute aucun oracle.

## PER04-R05 — Persistance de cmd_last_*

**Classification : `NOT_DEFINED`.**

La V1 définit un historique minimal de la dernière commande terminée, mais ne qualifie pas cet historique de non volatil ni de persistent à travers reboot.

## PER04-R06 — Persistance de cmd_last_timestamp

**Classification : `NOT_DEFINED`.**

Le timestamp est mis à jour lors d'une terminaison nominale selon FT-CMD-03, mais aucune règle post-reboot n'est définie.

## PER04-R07 — État post-reboot de cmd_active_*

**Classification : `NOT_DEFINED`.**

Aucune valeur initiale ou politique de conservation/réinitialisation post-reboot n'est donnée.

## PER04-R08 — État post-reboot de cmd_status / cmd_result_*

**Classification : `NOT_DEFINED`.**

Aucun état de boot obligatoire n'est défini pour ces champs.

## PER04-R09 — État post-reboot des champs de requête

**Classification : `NOT_DEFINED`.**

La V1 ne précise pas si `cmd_request_code`, paramètres, transaction_id, confirmation ou contrôle sont conservés, remis à zéro ou reconstruits.

## PER04-R10 — Commande interrompue par reset imprévu

**Classification : `NOT_DEFINED`.**

Pour watchdog, brown-out, reset externe ou coupure d'alimentation, aucune politique normative n'impose reprise, abandon, échec enregistré, rollback ou replay.

## PER04-R11 — RESET SOFTWARE avec opération critique inachevée

**Classification : `DELEGATED` vers FT-CMD-07.**

Le RESET SOFTWARE n'est admissible qu'en l'absence d'opération critique inachevée. La définition exhaustive des opérations critiques et le code de refus exact restent hors FT-PER-04.

## PER04-R12 — Observation B5 avant/après reboot

**Classification : `TRACE_ONLY`.**

Une campagne peut relever les champs B5 avant/après reboot afin de caractériser l'implémentation. Les observations ne deviennent pas des exigences V1.

## Règles anti-fabrication

- « historique » ne signifie pas automatiquement « persistant » ;
- « idempotent » ne signifie pas automatiquement « persistant à travers reboot » ;
- aucun état initial B5 n'est inventé ;
- aucun mécanisme automatique de reprise ou rollback n'est inventé ;
- aucun verdict FAIL n'est porté sur une simple différence post-reboot sans oracle explicite.
