# FT-STR-07 — Fiche de spécification

## Stabilité d’image et cohérence temporelle

## 1. Identification

- **ID** : FT-STR-07
- **Famille** : FT-STR
- **Criticité** : P0

## 2. Objectif

Valider que l’image Modbus est exploitable sans ambiguïté temporelle :
- les données réellement statiques restent stables tant que leur contexte ne change pas ;
- chaque réponse multi-registres est cohérente à un même instant logique ;
- les `uint32` ne peuvent pas être reconstruits à partir d’un MSW et d’un LSW provenant de deux instants différents ;
- le mode de découpage des lectures n’introduit aucun effet de bord.

## 3. Règles normatives appliquées

La règle critique de `charte_typage.md` s’applique à toute lecture multi-registres : tous les registres retournés doivent correspondre à un **même instant logique**.

Elle implique notamment :
- cohérence intra-champ `uint32` ;
- cohérence inter-champs d’une même réponse ;
- absence de mélange ancien/nouveau dans une même réponse.

## 4. Stabilité entre requêtes

L’identité bit à bit entre plusieurs requêtes successives n’est exigée que pour une cible dont la stabilité est explicitement garantie par V1 ou par un scénario de test maîtrisé.

Il est interdit de déclarer un champ « stable » uniquement parce qu’il varie peu ou parce que le capteur ne reçoit aucune commande externe.

Les données naturellement dynamiques, notamment temps, uptime, compteurs, états, mesures et indicateurs, peuvent évoluer normalement entre deux requêtes.

## 5. Atomicité `uint32`

Une lecture d’un `uint32` doit toujours retourner un couple MSW/LSW cohérent. Un test probant doit, lorsque possible, observer ou provoquer une évolution contrôlée de la valeur. Une valeur immobile ne suffit pas à démontrer l’atomicité.

## 6. Cohérence inter-champs

Pour une réponse contenant plusieurs champs dynamiques, les valeurs doivent appartenir au même instant logique.

La preuve doit reposer sur un oracle observable : compteur de séquence, timestamp associé, relation normative entre champs, instrumentation firmware ou mécanisme équivalent.

En l’absence d’oracle suffisant, le résultat est : **NON DÉMONTRABLE PAR L’INTERFACE SEULE / INSTRUMENTATION REQUISE**. Une simple plausibilité des valeurs ne vaut pas preuve.

## 7. Découpage des lectures

Les règles de FT-STR-06 restent applicables. En particulier :
- FC03 est limitée à 125 registres par requête ;
- le Bloc 4 (176 registres) ne peut pas être assimilé à un snapshot atomique complet obtenu par plusieurs requêtes successives ;
- plusieurs requêtes segmentées ne constituent pas, par elles-mêmes, un même instant logique.

## 8. Périmètre

Inclus :
- données statiques explicitement identifiées ;
- zones réservées ;
- `uint32` ;
- réponses multi-registres dynamiques ;
- alternance de modes de lecture valides.

Exclus :
- dynamique métier elle-même ;
- performance bus ;
- contention multi-maître ;
- persistance après reboot ;
- droits d’écriture.

## 9. Préconditions

- FT-STR-03, FT-STR-05 et FT-STR-06 gelées ;
- mapping GEL-MAP-V1 disponible ;
- scénario de test définissant les éventuelles cibles statiques ;
- moyen d’observation suffisant pour les tests de cohérence dynamique.

## 10. Critères de conformité

Conforme si :
- aucune cible déclarée statique ne varie hors événement explicatif ;
- aucun `uint32` déchiré n’est observé ;
- aucune réponse multi-registres ne mélange deux instants logiques ;
- le découpage de lecture n’altère ni l’état ni la cohérence exposée.

Toute conclusion non démontrable doit rester explicitement non démontrée, jamais transformée en PASS implicite.