# FT-CMD-05 — Validation détaillée

## TT-CMD-B05-400 — APPLY CONFIG admissible

Préparer une configuration complète, valide, avec CRC cohérent, acquisition arrêtée et `config_state = VALIDE`. Soumettre la commande 1 avec un `transaction_id` frais.

Oracle FT-CMD : la commande ne doit pas être refusée pour les causes `3`, `4`, `5` ou `20` ; le traitement transactionnel doit pouvoir aboutir normalement.

Note : la bascule effective de la configuration active appartient à FT-INT et n'est pas dupliquée ici.

---

## TT-CMD-B05-401 — APPLY refusée acquisition en cours

Préconditions : configuration autrement admissible, acquisition active.

Oracle : refus avec `cmd_result_code = 5`.

---

## TT-CMD-B05-402 — APPLY refusée configuration préparée incomplète

Construire explicitement un état préparé incomplet selon les règles du Bloc 4, sans introduire simultanément une autre cause de refus prioritaire.

Oracle : refus avec `cmd_result_code = 20`.

---

## TT-CMD-B05-403 — APPLY refusée configuration invalide / CRC incohérent

Cas A : configuration préparée invalide au sens normatif du Bloc 4.

Cas B : configuration autrement valide mais `prepared_config_crc` incohérent avec le recalcul effectué au moment de l'application.

Oracle : application refusée ; `cmd_result_code = 4` retenu pour configuration invalide/incohérente.

Restriction : ne pas inventer de code spécifique CRC inexistant dans la table V1.

---

## TT-CMD-B05-404 — APPLY refusée état incompatible

Classification : `CONDITIONAL` lorsque le banc peut construire un état incompatible distinct de l'acquisition en cours, de l'incomplétude et de l'invalidité.

Oracle : `cmd_result_code = 3`.

Si aucun cas distinct ne peut être établi sans hypothèse, le test reste non exécutable et doit être rapporté comme tel.

---

## TT-CMD-B05-405 — SYNC TIME admissible

Préparer une heure dans le Bloc 2, vérifier qu'elle est disponible et que l'horloge est disponible, puis soumettre la commande 2.

Oracle FT-CMD : absence de refus `12` ou `19`; la commande doit pouvoir être acceptée et traitée.

Les changements de temps effectif restent FT-INT.

---

## TT-CMD-B05-406 — SYNC refusée sans temps préparé

Établir `prepared_time_status = 0` / absence de temps préparé selon l'interface normative, puis soumettre la commande 2.

Oracle : refus avec `cmd_result_code = 19`.

---

## TT-CMD-B05-407 — SYNC refusée horloge indisponible

Établir un état normativement identifiable d'horloge indisponible tout en conservant un temps préparé présent.

Oracle : refus avec `cmd_result_code = 12`.

Si le banc ne permet pas de provoquer/identifier l'indisponibilité de l'horloge sans instrumentation ou mécanisme défini, l'exécution pratique peut être conditionnelle, mais l'oracle V1 reste défini.

---

## Règles communes

- utiliser un `transaction_id` frais ;
- ne pas exiger l'observation de tous les états intermédiaires `cmd_status` ;
- ne pas utiliser `cmd_result_detail` comme oracle générique ;
- ne pas revalider ici les effets inter-blocs déjà affectés à FT-INT.
