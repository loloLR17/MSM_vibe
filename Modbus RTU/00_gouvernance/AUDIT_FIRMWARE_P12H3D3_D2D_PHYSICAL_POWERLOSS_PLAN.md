# P12-H3d3-D2-D — Plan de qualification power-loss physique H3d2 / FRAM

## Statut

**PLAN DE QUALIFICATION — NON GELÉ**

Cette tranche part du gel D2-C et ne modifie pas les invariants H3d2.

## 1. Objectif

Qualifier sur STM32U575 + FRAM réelle le comportement de recovery après une coupure d'alimentation réelle à des points contrôlés du protocole de commit H3d2.

La baseline physique de départ est :

- autorité `VALID` ;
- génération 2 ;
- image B ;
- payload logique offset 0 = `0xA5`.

La génération candidate suivante sera donc génération 3 sur image A.

## 2. Principe de sécurité

Aucune coupure ne sera faite « au hasard ».

Un adaptateur de qualification, limité à la composition STM32, interceptera les appels `TransactionalImagePhysicalStorage.write` sans modifier le moteur transactionnel portable H3d2 ni le driver FRAM.

À un point de coupure armé, l'adaptateur :

1. exécute exactement la partie physique prévue pour le scénario ;
2. place une sonde volatile explicite ;
3. donne un signal visible de qualification ;
4. se bloque volontairement avant que le moteur H3d2 puisse poursuivre ;
5. l'opérateur coupe alors réellement l'alimentation ;
6. le firmware de qualification est ensuite désarmé avant l'observation de recovery lorsque le scénario l'exige.

Le blocage logiciel n'est pas lui-même la coupure qualifiée : la qualification porte sur la perte réelle d'alimentation qui suit.

Aucun retry automatique n'est autorisé.

## 3. Points de coupure retenus

L'ordre H3d2 gelé est :

1. payload complet de l'image inactive ;
2. header final de cette image ;
3. validation complète de l'image ;
4. superbloc de publication opposé ;
5. validation de la publication ;
6. bascule d'autorité runtime.

La campagne physique sera progressive.

### D2-D1 — payload image inactive partiellement écrit

Écrire seulement un préfixe contrôlé du payload de l'image A, puis bloquer avant la fin de l'appel physique.

Après coupure réelle et reboot, l'ancienne publication génération 2 / image B doit rester autorité et restituer `0xA5`.

Ce scénario vérifie qu'une image inactive partiellement réécrite sans header final/publication ne remplace pas l'autorité précédente.

### D2-D2 — payload complet, avant header final

Laisser l'écriture du payload de l'image A se terminer, puis bloquer avant l'écriture du header final.

Après coupure/reboot, génération 2 / image B doit rester autorité.

### D2-D3 — image génération 3 finalisée et validable, avant publication

Laisser terminer payload + header image A, puis bloquer avant l'écriture du nouveau superbloc.

Après coupure/reboot, génération 2 / image B doit rester autorité : une image nouvelle non publiée n'est pas autorité.

### D2-D4 — publication physiquement écrite

Laisser écrire le nouveau superbloc génération 3 / image A puis provoquer la coupure avant retour normal complet du scénario de qualification.

Après reboot, recovery peut légitimement sélectionner la nouvelle autorité si le superbloc et l'image sont physiquement valides. L'observation doit être décrite factuellement ; aucune réussite ne sera inférée d'un retour logiciel qui n'a pas eu lieu.

## 4. Mutation de campagne

Pour distinguer sans ambiguïté ancienne et nouvelle autorité :

- ancienne valeur connue : offset 0 = `0xA5` ;
- valeur candidate D2-D : offset 0 = `0x5A`.

Résultats attendus :

- autorité ancienne : génération 2 / image B / `0xA5` ;
- autorité nouvelle publiée : génération 3 / image A / `0x5A`.

Chaque scénario devra repartir d'une baseline explicitement observée. Aucun scénario suivant ne sera lancé sur une baseline supposée.

## 5. Instrumentation

L'instrumentation doit rester hors du moteur portable `transactional_image_media.c`.

Elle doit :

- être compilée uniquement derrière une porte de qualification explicite ;
- conserver le backend FRAM réel ;
- intercepter uniquement le contrat physique `read/write` ;
- ne pas modifier le format physique ;
- ne pas fabriquer de succès H3d2 ;
- exposer des sondes GDB du scénario armé et du point atteint ;
- fournir un signal LED distinct du clignotement normal avant coupure ;
- être désarmée après chaque scénario avant toute conclusion de recovery.

## 6. Critères D2-D1

Première tranche à implémenter :

1. vérifier baseline `VALID / gen2 / B / 0xA5` ;
2. préparer candidate offset 0 = `0x5A` ;
3. lancer un commit unique ;
4. intercepter l'écriture du payload vers l'image inactive A ;
5. écrire un préfixe strictement inférieur à 51 818 octets ;
6. signaler le point atteint et bloquer ;
7. couper réellement l'alimentation ;
8. désarmer le scénario ;
9. recompiler/flasher sans toucher à la FRAM ;
10. reboot/recovery ;
11. exiger `VALID / gen2 / B / 0xA5`.

## 7. Ce qui n'est pas encore qualifié

Le présent document ne constitue aucune preuve matérielle.

Ne sont pas encore qualifiés :

- D2-D1 à D2-D4 ;
- timing exact de la coupure ;
- brown-out ou rampes d'alimentation ;
- comportement électrique du bus pendant la chute de tension ;
- coupure pendant une transaction SPI individuelle ;
- endurance FRAM ;
- fault injection SPI.

## 8. Règle de progression

D2-D1 doit être observée et documentée avant d'armer D2-D2.

Une anomalie de recovery arrête la campagne : pas de formatage, réparation, retry ou passage au scénario suivant avant analyse de l'état physique.
