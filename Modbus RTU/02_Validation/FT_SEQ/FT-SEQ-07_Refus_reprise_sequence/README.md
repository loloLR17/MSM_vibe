# FT-SEQ-07 — Refus puis reprise de séquence

## 1. Objet

Valider les reprises fonctionnelles où une commande est refusée pour une cause V1 explicite, où cette cause est corrigée par une action normative, puis où une nouvelle tentative aboutit avec succès.

FT-SEQ-07 ne duplique pas FT-CMD : le refus et son code restent des oracles FT-CMD. La propriété FT-SEQ est la continuité `refus → correction → reprise → succès`.

## 2. Scénarios propriétaires

Quatre tests sont retenus :
- `TT-SEQ-REC-001` — START refusé sans configuration active valide → application d'une configuration → START réussi ;
- `TT-SEQ-REC-002` — SYNC TIME refusé sans temps préparé → préparation → synchronisation réussie ;
- `TT-SEQ-REC-003` — STOP refusé acquisition inactive → START valide → STOP réussi ;
- `TT-SEQ-REC-004` — APPLY CONFIG refusé pendant acquisition → STOP → application réussie.

## 3. Couverture

- `COVERED` propriétaire FT-SEQ : 4
- `CONDITIONAL` : 0
- `DELEGATED` : oracles élémentaires vers FT-CMD / FT-INT / FT-SEQ-02..05
- `TRACE_ONLY` : 3 chaînes de récupération non reproductibles génériquement en V1
- `NOT_DEFINED` : 4 politiques de retry/temporisation/priorité

## 4. Chaînes non transformées en tests FT-SEQ V1

Ne sont pas créées comme procédures génériques :
- code 12 horloge indisponible → récupération ;
- code 8 défaut critique → suppression de cause ;
- codes 6/7 SD absente/mémoire insuffisante → restauration.

Les refus sont normatifs, mais V1 ne définit pas de procédure corrective générique et reproductible pour ces causes. Les transformer en tests de reprise imposerait une procédure absente de la spécification.

## 5. Doctrine de retry

FT-SEQ-07 n'impose :
- ni nombre de tentatives ;
- ni backoff ;
- ni délai minimal/maximal ;
- ni réutilisation du même `transaction_id`.

Les règles transactionnelles restent FT-CMD. Une reprise FT-SEQ doit démontrer la correction de la cause avant de retenter ; répéter aveuglément la commande n'est pas une reprise fonctionnelle valide.

## 6. Frontières

- refus/code isolé : FT-CMD ;
- effets inter-blocs : FT-INT ;
- chaînes nominales : FT-SEQ-02 à 06 ;
- reboot/persistance : FT-PER ;
- pertes/répétitions/délais hostiles : FT-RBT.

Voir `source/FT-SEQ-07_source.md` et les quatre tests sous `detaille/`.