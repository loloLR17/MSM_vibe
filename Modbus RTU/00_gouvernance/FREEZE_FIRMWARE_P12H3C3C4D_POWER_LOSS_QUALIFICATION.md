# FREEZE — P12-H3c3-C4D — Qualification transactionnelle et power-loss du CommandJournal borné V3

## Statut

Tranche C4-D gelée après revue stricte et validation locale utilisateur.

Validation déclarée :
- CTest : 84/84 verts ;
- HOST VALIDATED ;
- CROSS-BUILD VALIDATED ;
- HARDWARE PENDING.

C4-D qualifie le store V3 borné isolé gelé en C4-C5. La bascule du runtime actif V2 vers V3 n'est pas incluse dans ce gel.

## Référence de qualification

La matrice normative de travail est :
- CONCEPTION_FIRMWARE_P12H3C3C4D0_POWER_LOSS_MATRIX.md

Les tranches automatisées D1 à D8 couvrent le modèle de faute défini par cette matrice.

## D1 — média de faute

Le banc de test distingue :
- write partiel à N octets ;
- image working et image durable ;
- power-cycle durable -> working ;
- commit réussi ;
- commit échoué avec rollback ;
- commit échoué laissant les octets seulement lisibles mais non durables.

Les tranches intégrées complètent ce modèle avec le cas où commit retourne une erreur alors que les nouveaux octets sont néanmoins devenus durables.

## D2 — admission initiale EMPTY -> RESERVED

Qualification :
- coupures 0, 1, avant CRC, CRC partiel et fin de record ;
- aucune image partielle acceptée après reboot ;
- admission durable retrouvée RESERVED ;
- next_admission_order reconstruit depuis le média ;
- commit ambigu non durable : ancienne image vide ;
- commit ambigu durable : nouvelle transaction retrouvée après reboot bien que l'appel ait retourné une erreur ;
- runtime fail-closed après erreur, sans retry automatique.

## D3 — mutations recovery_context et RESERVED -> STARTED

Qualification A/B :
- copie précédente conservée si nouvelle copie déchirée ;
- copie plus récente retenue si entièrement durable ;
- admission_order conservé ;
- génération croissante ;
- aucun état synthétique.

Cette tranche regroupe les deux transitions prévues séparément comme D2/D3 dans la matrice initiale.

## D4 — completion

Qualification STARTED -> COMPLETED :
- copie STARTED reste autoritaire si COMPLETED est déchirée ;
- COMPLETED durable conserve final_result, result_detail, terminal_timestamp et completion_order ;
- next_completion_order reconstruit après reboot ;
- commit en erreur ferme le runtime et n'avance pas le compteur runtime.

## D5 — éviction / réadmission

Avec 256 slots occupés :
- victime = plus ancien COMPLETED éligible ;
- nouvelle copie partielle : ancienne transaction conservée ;
- nouvelle copie durable : nouvelle transaction RESERVED retenue ;
- jamais d'hybride entre victime et nouvelle transaction ;
- generation croissante ;
- nouvel admission_order uniquement pour la nouvelle admission retenue ;
- next_admission_order reconstruit.

## D6 — sélection et corruption A/B

Qualification cumulée par C1/C2 et consolidation D6 :
- une copie valide masque une pair corrompue ;
- deux copies invalides -> CORRUPTED ;
- deux copies valides de générations différentes -> génération la plus récente ;
- même génération, contenu différent -> CORRUPTED ;
- même génération, contenu identique -> VALID ;
- UNSUPPORTED et UNAVAILABLE sont classifiés ;
- priorité globale recovery : UNAVAILABLE > UNSUPPORTED > CORRUPTED.

La revue C4-D a supprimé une dépendance au padding C : l'égalité de deux records de même génération est maintenant déterminée par leur encodage V3 canonique de 70 octets, et non par memcmp brut de la structure décodée.

## D7 — idempotence après reboot

Pour un ID encore retenu dans la fenêtre :
- même transaction_id + même request_identity : retry existant, aucune nouvelle écriture ni commit ;
- même transaction_id + identité différente : collision/refus, aucune nouvelle écriture ni commit ;
- le journal existant reste autoritaire.

La réexécution métier n'est pas déclenchée par le store ; la sémantique complète reste portée par CommandEngine et sera requalifiée lors de la bascule C4-E.

## D8 — non-terminal unique

Recovery accepte zéro ou un record logique RESERVED/STARTED.
Deux non-terminaux logiques distincts classifient l'image CORRUPTED et maintiennent le store fermé.

## Invariants power-loss gelés

Après toute erreur persistante :
- recovery_required est réarmé ;
- aucune mutation n'est automatiquement rejouée ;
- les compteurs runtime ne supposent pas la réussite physique ;
- après reboot, le média durable, le sélecteur A/B et le recovery scan sont l'unique autorité ;
- une écriture déchirée ne peut devenir un record valide sans CRC/codec valides ;
- aucune bascule hybride V2/V3 n'est admise.

## Hors périmètre

Restent hors C4-D :
- bascule atomique V2 -> V3 dans SystemRuntime / CommandBootRecovery (C4-E) ;
- migration de données V2 existantes vers V3 ;
- choix et endurance du média persistant STM32 réel ;
- brown-out matériel réel ;
- validation sur NUCLEO-U575ZI-Q.

## Barrière C4-E

C4-E doit remplacer atomiquement la composition active :
- géométrie/region V2 dense ;
- CommandJournalStore V2 ;
- CommandJournalRecoveryResult V2 ;
- dépendance concrète V2 de CommandBootRecovery.

La bascule ne doit pas conserver un mélange V2/V3. Les tests runtime/B5/recovery existants doivent rester verts, puis des tests ciblés doivent prouver que le runtime actif utilise réellement le store V3 borné.
