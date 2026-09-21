# P12-H3c3-C4D0 — Matrice transactionnelle et power-loss du CommandJournal borné V3

## Objet

Qualifier le store V3 gelé en C4-C5 face aux coupures simulées et aux écritures incomplètes avant toute bascule V2 -> V3.

C4-D ne modifie pas la politique métier. Il constitue une barrière de qualification.

## Modèle de faute

Chaque mutation persistante suit :

1. lecture/sélection de la copie courante ;
2. construction du nouveau record ;
3. write de la copie cible ;
4. commit ;
5. seulement après succès, mise à jour des compteurs runtime.

Une coupure peut laisser :
- l'ancienne copie seule valide ;
- la nouvelle copie entièrement valide ;
- la nouvelle copie partielle/corrompue ;
- un write réussi avec commit non confirmé.

Au reboot, le sélecteur A/B et le recovery scan sont l'unique autorité. Aucun retry automatique de mutation n'est autorisé.

## Matrice minimale C4-D

### D1 — admission initiale EMPTY -> RESERVED

Injecter :
- coupure/échec avant write ;
- write partiel de la copie 0 ;
- write complet puis commit en erreur ;
- succès complet.

Attendus après reboot :
- image vide ou transaction RESERVED valide selon ce qui est réellement durable ;
- jamais de record partiel accepté ;
- compteur admission reconstruit depuis le média ;
- aucune hypothèse fondée sur le compteur runtime précédent.

### D2 — mutation RESERVED -> recovery_context

Avec copie A valide :
- copie B partielle/corrompue ;
- copie B complète plus récente ;
- commit en erreur après write complet.

Attendus :
- A reste sélectionnable si B invalide ;
- B est sélectionnée si valide et génération supérieure ;
- contexte jamais synthétisé ;
- recovery cohérent avec l'image durable.

### D3 — RESERVED -> STARTED

Même matrice A/B que D2.

Attendus :
- après reboot, état exactement RESERVED ou STARTED selon la copie valide la plus récente ;
- jamais de double transaction ;
- admission_order inchangé.

### D4 — STARTED/RESERVED -> COMPLETED

Injecter coupure autour de l'écriture de la copie opposée.

Attendus :
- ancien état non terminal ou nouveau COMPLETED ;
- si COMPLETED durable, final_result, timestamp et completion_order sont cohérents ;
- next_completion_order reconstruit ;
- aucune double completion.

### D5 — éviction/réadmission COMPLETED -> nouveau RESERVED

Store plein, victime terminale connue.

Injecter :
- nouvelle copie partielle ;
- nouvelle copie complète ;
- commit en erreur après write complet.

Attendus :
- ancienne transaction ou nouvelle transaction, jamais un hybride ;
- generation strictement croissante ;
- nouvel admission_order uniquement si nouvelle transaction retenue ;
- compteur admission reconstruit.

### D6 — corruption des copies

Vérifier :
- une copie valide + pair corrompue : copie valide retenue ;
- deux copies invalides : recovery CORRUPTED ;
- deux copies valides, générations différentes : plus récente retenue ;
- générations égales et contenu différent : CORRUPTED ;
- erreur média : UNAVAILABLE prioritaire.

### D7 — idempotence après reboot

Pour une transaction encore dans la fenêtre :
- même ID + même request_identity après reboot : aucune réexécution ;
- même ID + identité différente : collision/refus ;
- transaction COMPLETED conserve son résultat antérieur.

### D8 — non-terminal unique

Après toute faute simulée et recovery :
- au plus un record logique RESERVED/STARTED ;
- deux non-terminaux logiques distincts doivent classifier l'image CORRUPTED.

## Injection partielle

Les tests C4-D doivent pouvoir limiter une écriture à N octets puis simuler une coupure, au lieu de seulement retourner une erreur avant écriture. Les valeurs N doivent couvrir au minimum :
- 0 octet ;
- 1 octet ;
- avant CRC ;
- CRC partiel ;
- record complet.

L'objectif est de qualifier le codec + CRC + sélection A/B face à une vraie image déchirée.

## Commit ambigu

Le média de test doit pouvoir simuler séparément :
- commit échoue et le write précédent n'est pas durable ;
- commit échoue mais le write précédent reste lisible/durable.

Le store doit retourner une erreur et exiger recovery dans les deux cas. Après reboot, seul le contenu effectivement présent décide de l'état.

## Critères de sortie C4-D

C4-D est validé seulement si :
- la matrice ci-dessus est automatisée ;
- aucune modification des tests V2 n'est utilisée pour masquer un écart ;
- tous les tests host sont verts ;
- cross-build STM32 reste vert ;
- aucune bascule V2 -> V3 n'est réalisée ;
- les éventuelles anomalies de conception sont corrigées avant le freeze C4-D.

## Hors périmètre

- endurance réelle Flash/EEPROM ;
- brown-out électrique réel ;
- média STM32 de production ;
- timing matériel ;
- bascule C4-E.
