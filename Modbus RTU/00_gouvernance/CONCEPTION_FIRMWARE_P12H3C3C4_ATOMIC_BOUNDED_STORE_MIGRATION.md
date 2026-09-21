# P12-H3c3-C4 — Migration atomique du CommandJournalStore borné

## 1. Constat issu de C3-B0

Le raccordement isolé de find, visit et latest_completed au reader V3 a démontré qu'un CommandJournalStore hybride n'est pas valide : les écritures et le recovery V2 dense ne peuvent pas cohabiter avec des lectures V3 bornées.

La migration du store doit donc être réalisée comme une composition cohérente, sans état intermédiaire V2-écriture / V3-lecture dans la baseline validée.

## 2. Principe

Les briques V3 déjà validées restent indépendantes :
- record V3 ;
- sélection A/B ;
- recovery C2 ;
- reader C3.

Le CommandJournalStore actif reste V2 jusqu'à ce que les opérations V3 de mutation soient disponibles et testées.

Le basculement final du store n'intervient qu'après validation isolée de l'ensemble V3.

## 3. Découpage

### C4-A — writer/mutator V3 isolé
Créer une brique indépendante du CommandJournalStore historique pour :
- écrire une nouvelle admission dans un slot logique choisi ;
- muter une entrée existante ;
- écrire exclusivement la copie opposée à la copie courante ;
- utiliser generation + 1 ;
- préserver admission_order lors des mutations ;
- refuser toute mutation si generation == UINT32_MAX ;
- commit après write ;
- ne jamais masquer une erreur write/commit.

Aucune politique de choix de slot ni d'éviction dans C4-A.

### C4-B — admission planner isolé
Déterminer, sans écrire :
- transaction déjà présente ;
- slot EMPTY disponible ;
- sinon plus ancien COMPLETED selon admission_order ;
- jamais RESERVED/STARTED comme victime ;
- slot à generation UINT32_MAX non réutilisable ;
- absence de victime admissible -> refus explicite.

### C4-C — bounded journal store isolé
Composer recovery + reader + planner + writer derrière le contrat CommandJournal, sans remplacer encore le store V2 utilisé par SystemRuntime.

### C4-D — tests transactionnels et power-loss
Exercer retry, collision, 256/257, éviction, réutilisation après éviction, wrap central 65535->1, recovery et défauts write/commit.

### C4-E — bascule atomique
Seulement après validation de C4-A à C4-D :
- remplacer le store dense V2 ;
- supprimer le couplage max_transaction_id ;
- adapter les call sites ;
- exécuter la suite complète Host + Cortex-M33.

## 4. Invariants C4-A

Pour un slot VALID sélectionné sur copy N :
- destination = 1 - N ;
- new_generation = old_generation + 1 ;
- generation ne wrap jamais ;
- l'ancienne copie n'est pas effacée avant validation de la nouvelle ;
- un échec write ou commit laisse l'appel en erreur ;
- la sélection A/B au reboot reste l'autorité.

Pour un slot EMPTY :
- première copie cible déterministe : copy 0 ;
- generation initiale = 1 ;
- admission_order doit être fourni par l'appelant et non inventé par le writer.

Une ré-admission après éviction n'est pas un slot EMPTY : elle continue la génération du slot physique et écrit la copie opposée.

## 5. Portée normative

Aucun changement du mapping B5, du type uint16 transaction_id ni des règles métier gelées H3c1.

Le changement porte uniquement sur la représentation persistante et la politique de rétention bornée déjà arbitrées.

## 6. Prochaine micro-tranche

C4-A1 : définir le contrat public interne du writer V3, sans implémentation et sans modifier CommandJournalStore.
