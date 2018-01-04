#Yore ideas

This is a disorganised set of thoughts on Yore.

<pre>
    Back in the days of Yore, in a land far removed
    from our current understanding of the universe,
     when magic flowed throughout the air as water
     flowed through the sea, and the Gods lived in
      harmony with the people; it was a time when
        anything and everything was possible...
</pre>

Monsters noticing things should be their own events, so we can keep track of what each monster knows. Probably don't need to keep much persistent knowledge for monsters, this would just be for instantaneous things like angering.

Text input: non-text mode should not use SDL's thingo except for punctuation. Capitals can be rendered in-house. Then normal keys will correspond with control-keys and things like hjklyubn will be consistently located across-platoform. Also add key-rebinding capabilities in like an options menu.

Turn events into a heap.

Monsters should turn hostile if near to an altercation with the player. Use area maps for monster AI?

Ability to wait for a set number of ticks

##How spellcasting works (will work)

You have belt studded with jewels (maybe 5? maybe longer belts exist but are exponentially rarer/more expensive) in a row. Each jewel has an 'effect'.
When you cast a spell the effect of each jewel in turn is applied to the spell until it is finally cast at your opponent.
Thoughts on what the jewels could do:

 * increase elemental/general damage/defense
 * moves something (eg you/your target)
 * changed attack type
 * add a confusion attack
 * summon something
 * all the above with an added cost (eg HP loss, confusion, etc) depending on how OP it is

Could do it like NH - have a set list of descriptions of jewels ('red', 'black', 'striped', etc), and at game start randomly assign each one to a function
('double fire damage', 'triple damage against undead', etc). How would the player identify the jewels? Naming? Messages (like 'This fire attack feels twice as powerful!')? Monster use?

##Graphics

I think the reason it's laggy is because update_stats is called once per move *per monster*.
So, at the beginning everything is fast; as monsters are generated (which you can't even see) they slow you down.
Evidence: as I was playing every few turns my movements seemed to slow down slightly.
gr_refresh, I suspect, has basically no impact on speed.

I'm not sure how to fix this though; some optimisation in visualise_map is certainly called for.
Maybe check each time all_things is modified if it will affect the screen; then only those squares which have changed need to be gone through in visualise_map.
Alternatively if anything visible has changed call visualise_map normally, otherwise don't call it at all.
Easier, simpler and probably just as quick. Monsters moving out of sight won't impact on speed.
Even cruder: only call update_stats at the end of each turn. I think I'll do that, for now. In fact, it works excellently. Yay :)

##That niche bug

There was this bug (fixed as of this commit) where when an item was picked up from a niche (a space where there should be wall) Yore crashed the next turn.
I fixed it by putting a lit space in the same square as a niche -- so when you picked up something there was something there still, not just "empty space".
The funny thing was, when I put the lit space *after* the item in the list, Yore still crashed. Reordering the things in the list changed its behaviour. How weird is that?
So I figure:

 * there is a bug in list_rem, or
 * there is a bug in ITER_THING[S], or
 * everything is totally messed up. :(
